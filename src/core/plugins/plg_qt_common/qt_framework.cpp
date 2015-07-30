#include "qt_framework.hpp"

#include "data_model/i_item_role.hpp"

#include "qt_common/i_qt_type_converter.hpp"
#include "qt_common/qml_component.hpp"
#include "qt_common/qml_view.hpp"
#include "qt_common/qt_palette.hpp"
#include "qt_common/qt_default_spacing.hpp"
#include "qt_common/qt_global_settings.hpp"
#include "qt_common/qt_window.hpp"
#include "qt_common/string_qt_type_converter.hpp"
#include "qt_common/qt_image_provider.hpp"
#include "qt_script/qt_scripting_engine.hpp"
#include "qt_script/qt_script_object.hpp"
#include "ngt_core_common/environment.hpp"

#include "serialization/interfaces/i_file_utilities.hpp"

#include "generic_plugin/interfaces/i_component_context.hpp"
#include "generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "command_system/i_command_event_listener.hpp"
#include "command_system/i_command_manager.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_component_provider.hpp"
#include "ui_framework/generic_component_provider.hpp"

#include "wg_types/string_ref.hpp"

#include <array>
#include <QApplication>
#include <QFile>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QString>
#include <QWidget>

namespace QtFramework_Locals
{
	// Temporary command event listener to handle process events when the command
	// thread blocks
	class QtCommandEventListener : public ICommandEventListener
	{
	public:
		void progressMade( const CommandInstance & commandInstance ) const override
		{
			QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
		}
	};
}

QtFramework::QtFramework()
	: qmlEngine_( new QQmlEngine() )
	, scriptingEngine_( new QtScriptingEngine() )
	, palette_( new QtPalette() )
	, defaultQmlSpacing_( new QtDefaultSpacing() )
	, globalQmlSettings_( new QtGlobalSettings() )
{

	char ngtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>( "NGT_HOME", ngtHome ))
	{
		qmlEngine_->addPluginPath( ngtHome );
		qmlEngine_->addImportPath( ngtHome );
	}
}

QtFramework::~QtFramework()
{
}

void QtFramework::initialise( IComponentContext & contextManager )
{
	// This needs to be set after qtFramework has been constructed and QmlEngine has been created.
	// This will only occur when running from a plugin scenario such as Maya.
	IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();
	if (pPluginContextManager && pPluginContextManager->getExecutablePath())
	{
		qmlEngine_->addPluginPath(pPluginContextManager->getExecutablePath());
		qmlEngine_->addImportPath(pPluginContextManager->getExecutablePath());
	}

	Q_INIT_RESOURCE( qt_common );

	registerDefaultComponents();
	registerDefaultComponentProviders();
	registerDefaultTypeConverters();

	scriptingEngine_->initialise( *this, contextManager );

	auto rootContext = qmlEngine_->rootContext();
	rootContext->setContextObject( scriptingEngine_.get() );
	rootContext->setContextProperty( "palette", palette_.get() );
	rootContext->setContextProperty( "defaultSpacing", defaultQmlSpacing_.get() );
	rootContext->setContextProperty( "globalSettings", globalQmlSettings_.get() );

	qmlEngine_->addImportPath( "qrc:/" );
	qmlEngine_->addImageProvider( 
		QtImageProvider::providerId(), new QtImageProvider() );

	auto commandManager = contextManager.queryInterface< ICommandManager >();
	if (commandManager != nullptr)
	{
		commandEventListener_.reset( new QtFramework_Locals::QtCommandEventListener );
		commandManager->registerCommandStatusListener( commandEventListener_.get() );
	}
}

void QtFramework::finalise()
{
	qmlEngine_->removeImageProvider( QtImageProvider::providerId() );
	scriptingEngine_->finalise();

	globalQmlSettings_ = nullptr;
	defaultQmlSpacing_ = nullptr;
	palette_ = nullptr;
	qmlEngine_ = nullptr;
	scriptingEngine_ = nullptr;

	defaultTypeConverters_.clear();
	defaultComponentProviders_.clear();
	defaultComponents_.clear();

	Q_CLEANUP_RESOURCE( qt_common );
}

QQmlEngine * QtFramework::qmlEngine() const
{
	return qmlEngine_.get();
}

const QtPalette * QtFramework::palette() const
{
	return palette_.get();
}

QtGlobalSettings * QtFramework::qtGlobalSettings() const
{
	return globalQmlSettings_.get();
}

void QtFramework::registerTypeConverter( IQtTypeConverter & converter )
{
	typeConverters_.push_back( &converter );
}

QVariant QtFramework::toQVariant( const Variant & variant ) const
{
	QVariant qVariant( QVariant::Invalid );
	for (auto it = typeConverters_.rbegin(); it != typeConverters_.rend(); ++it)
	{
		if ((*it)->toQVariant( variant, qVariant ))
		{
			break;
		}
	}
	return qVariant;
}

Variant QtFramework::toVariant( const QVariant & qVariant ) const
{
	Variant variant;
	for (auto it = typeConverters_.rbegin(); it != typeConverters_.rend(); ++it)
	{
		if ((*it)->toVariant( qVariant, variant ))
		{
			break;
		}
	}
	return variant;
}

QQmlComponent * QtFramework::toQmlComponent( IComponent & component )
{
	// TODO replace this with a proper UI adapter interface
	auto qmlComponent = dynamic_cast< QmlComponent * >( &component );
	if (qmlComponent != nullptr)
	{
		return qmlComponent->component();
	}

	return nullptr;
}

QWidget * QtFramework::toQWidget( IView & view )
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast< QmlView * >( &view );
	if (qmlView != nullptr)
	{
		auto widget = qmlView->release();
		widget->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
		widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		widget->setFocusPolicy( Qt::StrongFocus );
		return widget;
	}

	auto widget = dynamic_cast< QWidget * >( &view );
	if (widget != nullptr)
	{
		return widget;
	}

	return nullptr;
}

std::unique_ptr< IAction > QtFramework::createAction(
	const char * id, std::function<void()> func, 
	std::function<bool()> enableFunc )
{
	return actionManager_.createAction( id, func, enableFunc );
}

std::unique_ptr< IComponent > QtFramework::createComponent( 
	const char * resource, ResourceType type )
{
	auto component = new QmlComponent( *qmlEngine_ );

	switch (type)
	{
	case IUIFramework::ResourceType::Buffer:
		component->component()->setData( resource, QUrl() );
		break;

	case IUIFramework::ResourceType::File:
		component->component()->loadUrl( QUrl::fromLocalFile( resource ) );
		break;

	case IUIFramework::ResourceType::Url:
		component->component()->loadUrl( QUrl( resource ) );
		break;
	}

	return std::unique_ptr< IComponent >( component );
}

std::unique_ptr< IView > QtFramework::createView( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	// TODO: This function assumes the resource is a qml file

	QUrl qUrl;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		qUrl = QUrl::fromLocalFile( resource );
		break;

	case IUIFramework::ResourceType::Url:
		qUrl = QUrl( resource );
		break;

	default:
		return nullptr;
	}

	auto scriptObject = scriptingEngine_->createScriptObject( context );

	auto view = new QmlView( *qmlEngine_ );

	if (scriptObject)
	{
		view->setContextObject( scriptObject );
	}
	else
	{
		auto source = toQVariant( context );
		view->setContextProperty( QString( "source" ), source );
	}

	view->load( qUrl );
	return std::unique_ptr< IView >( view );
}

std::unique_ptr< IWindow > QtFramework::createWindow( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	// TODO: This function assumes the resource is a ui file containing a QMainWindow

	std::unique_ptr< QIODevice > device;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			device.reset( new QFile( resource ) );
			device->open( QFile::ReadOnly );
		}
		break;

	default:
		return nullptr;
	}

	assert( device != nullptr );
	auto window = new QtWindow( *this, *device );
	device->close();

	return std::unique_ptr< IWindow >( window );
}

void QtFramework::loadActionData( const char * resource, ResourceType type )
{
	std::unique_ptr< QIODevice > device;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			device.reset( new QFile( resource ) );
			device->open( QFile::ReadOnly );
		}
		break;

	default:
		return;
	}

	assert( device != nullptr );
	actionManager_.loadActionData( *device );
	device->close();
}

void QtFramework::registerComponent( const char * id, IComponent & component )
{
	auto it = components_.find( id );
	if (it != components_.end())
	{
		return;
	}

	components_[id] = &component;
}

void QtFramework::registerComponentProvider( IComponentProvider & provider )
{
	componentProviders_.push_back( &provider );
}

IComponent * QtFramework::findComponent( const TypeId & typeId, 
	std::function< bool ( size_t ) > & predicate ) const
{
	for (auto componentProviderIt = componentProviders_.rbegin(); 
		componentProviderIt != componentProviders_.rend(); 
		++componentProviderIt)
	{
		auto id = ( *componentProviderIt )->componentId( typeId, predicate );
		if (id == nullptr)
		{
			continue;
		}

		auto componentIt = components_.find( id );
		if (componentIt != components_.end())
		{
			return componentIt->second;
		}
	}
	return nullptr;
}

void QtFramework::setPluginPath( const std::string& path )
{
	pluginPath_ = path;
}

const std::string& QtFramework::getPluginPath() const
{
	return pluginPath_;
}

void QtFramework::registerDefaultComponents()
{
	std::array<std::string, 11> types =
	{
		"boolean", "string", "int32", "enum", "slider", "polystruct",
		"vector3", "vector4", "color3", "color4", "thumbnail"
	};

	for (auto & type : types)
	{
		// TODO: FileUtilities is a dummy interface atm so this will not actually
		// resolve to a absolute path
		const std::string file = "ui/" + type + "_component.qml";
		const std::string url = "qrc:///WGControls/" + type + "_component.qml";

		if (std::unique_ptr< IComponent > component = QFile::exists( file.c_str() ) ? 
			createComponent( file.c_str(), ResourceType::File ) : 
			createComponent( url.c_str(), ResourceType::Url ))
		{
			defaultComponents_.emplace_back( std::move(component) );
			registerComponent( type.c_str(), *defaultComponents_.back() );
		}
	}
}

void QtFramework::registerDefaultComponentProviders()
{
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<char>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned char>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<short>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned short>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<int>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned int>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<long>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned long>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<long long>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned long long>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<float>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<double>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<const char *>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<const wchar_t *>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<std::string>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<std::wstring>( "string" ) );

	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<bool>( "boolean" ) );

	size_t enumRoles[] = { IsEnumRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "enum", enumRoles ) );
	size_t thumbnailRoles[] = { IsThumbnailRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "thumbnail", thumbnailRoles ) );
	size_t sliderRoles[] = { IsSliderRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "slider", sliderRoles ) );

	for (auto & defaultComponentProvider : defaultComponentProviders_)
	{
		registerComponentProvider( *defaultComponentProvider );
	}
}

void QtFramework::registerDefaultTypeConverters()
{
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<bool>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<int32_t>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<uint32_t>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<int64_t>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<uint64_t>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<float>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<double>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<std::shared_ptr< BinaryBlock >>() );
	defaultTypeConverters_.emplace_back( new StringQtTypeConverter() );
	for (auto & defaultTypeConverter : defaultTypeConverters_)
	{
		registerTypeConverter( *defaultTypeConverter );
	}
}
