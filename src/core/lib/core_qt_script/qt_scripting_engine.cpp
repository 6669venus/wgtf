#include "qt_scripting_engine.hpp"

#include "qobject_qt_type_converter.hpp"
#include "qt_script_object.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/controls/bw_copyable.hpp"
#include "script_qt_type_converter.hpp"
#include "wg_list_iterator.hpp"
#include "collection_qt_type_converter.hpp"

#include "core_reflection/base_property.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"

#include "core_command_system/i_command_manager.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_data_model/i_list_model.hpp"

#include <private/qmetaobjectbuilder_p.h>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>

Q_DECLARE_METATYPE( ObjectHandle );

QtScriptingEngine::QtScriptingEngine()
	: defManager_( nullptr )
	, commandSystemProvider_( nullptr )
	, copyPasteManager_( nullptr )
	, contextManager_( nullptr )
{
}

QtScriptingEngine::~QtScriptingEngine()
{
	assert( scriptObjects_.empty() );

	std::lock_guard< std::mutex > guard( metaObjectsMutex_ );

	for (auto& metaObjectPair: metaObjects_)
	{
		free( metaObjectPair.second );
	}

	metaObjects_.clear();
}

void QtScriptingEngine::initialise( 
	IQtFramework & qtFramework, IComponentContext & contextManager )
{	
	contextManager_ = &contextManager;
	defManager_ = contextManager.queryInterface< IDefinitionManager >();
	commandSystemProvider_ =
		contextManager.queryInterface< ICommandManager >();

	copyPasteManager_ = 
		contextManager.queryInterface<ICopyPasteManager>();

	uiApplication_ =
		contextManager_->queryInterface< IUIApplication >();
	assert( defManager_ );
	assert( commandSystemProvider_ );
	assert( copyPasteManager_ );
	assert( uiApplication_ );


	qtTypeConverters_.emplace_back( new GenericQtTypeConverter< ObjectHandle >() );
	qtTypeConverters_.emplace_back( new CollectionQtTypeConverter() );
	qtTypeConverters_.emplace_back( new QObjectQtTypeConverter() );
	qtTypeConverters_.emplace_back( new ScriptQtTypeConverter( *this ) );

	QMetaType::registerComparators< ObjectHandle >();
	for (auto & qtTypeConverter : qtTypeConverters_)
	{
		qtFramework.registerTypeConverter( *qtTypeConverter );
	}
}

void QtScriptingEngine::finalise()
{
	for (auto& scriptObject: scriptObjects_)
	{
		delete scriptObject.second;
	}

	scriptObjects_.clear();
}

QtScriptObject * QtScriptingEngine::createScriptObject( 
	const ObjectHandle & object )
{
	auto itr = scriptObjects_.find( object );

	if (itr != scriptObjects_.end())
	{
		return itr->second;
	}

	auto classDefinition = object.getDefinition();
	if (classDefinition == nullptr)
	{
		return nullptr;
	}

	auto metaObject = getMetaObject( *classDefinition );
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	assert( contextManager_ );
	QtScriptObject* scriptObject = new QtScriptObject(
		*contextManager_, *metaObject, object, nullptr );

	scriptObjects_.emplace( object, scriptObject );
	return scriptObject;
}

QObject * QtScriptingEngine::createObject( QString definition )
{
	auto className = std::string( "class " ) + definition.toUtf8().constData();

	if (defManager_ == nullptr)
	{
		qCritical( "Definition manager not found. Could not create object: %s \n", 
			className.c_str() );
		return nullptr;
	}

	auto classDefinition = defManager_->getDefinition( className.c_str() );
	if (classDefinition == nullptr)
	{
		qWarning( "No definition registered for type: %s \n", className.c_str() );
		return nullptr;
	}

	auto object = classDefinition->createManagedObject();
	if (object == nullptr)
	{
		qWarning( "Could not create C++ type: %s \n", className.c_str() );
		return nullptr;
	}

	// no parent as qml takes ownership of this object
	auto scriptObject = createScriptObject( object );
	if (scriptObject == nullptr)
	{
		qWarning( "Could not create Qt type: %s \n", className.c_str() );
		return nullptr;
	}

	return scriptObject;
}


bool QtScriptingEngine::queueCommand( QString command )
{
	auto commandId = std::string( "class " ) + command.toUtf8().constData();
	Command * cmd = commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Could not find Command: %s \n", commandId.c_str() );
		return false;
	}
	commandSystemProvider_->queueCommand( commandId.c_str() );
	return true;
}

void QtScriptingEngine::beginUndoFrame()
{
	commandSystemProvider_->beginBatchCommand();
}

void QtScriptingEngine::endUndoFrame()
{
	commandSystemProvider_->endBatchCommand();
}

void QtScriptingEngine::abortUndoFrame()
{
	commandSystemProvider_->abortBatchCommand();
}

void QtScriptingEngine::deleteMacro( QString command )
{
	std::string commandId = command.toUtf8().constData();
	Command * cmd = commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Delete macro failed: Could not find Macro: %s \n", commandId.c_str() );
		return;
	}
	commandSystemProvider_->deleteMacroByName( commandId.c_str() );
}

void QtScriptingEngine::selectControl( BWCopyable* control, bool append )
{
	copyPasteManager_->onSelect( control, append );
}

void QtScriptingEngine::deselectControl( BWCopyable* control, bool reset )
{
	copyPasteManager_->onDeselect( control, reset );
}

QObject * QtScriptingEngine::iterator( const QVariant & collection )
{
	int typeId = collection.type();
	if (typeId == QVariant::UserType)
	{
		typeId = collection.userType();
	}

	if (typeId != qMetaTypeId< ObjectHandle >())
	{
		return nullptr;
	}

	auto handle = collection.value< ObjectHandle >();
	if (!handle.isValid())
	{
		return nullptr;
	}

	auto listModel = handle.getBase< IListModel >();
	if (listModel == nullptr)
	{
		return nullptr;
	}

	// QML will take ownership of this object
	return new WGListIterator( *listModel );
}

bool QtScriptingEngine::setValueHelper( QObject * object, QString property, QVariant value )
{
	if (object == nullptr)
	{
		return false;
	}

	return object->setProperty( property.toUtf8(), value );
}

void QtScriptingEngine::closeWindow( const QString & windowId )
{
	std::string id = windowId.toUtf8().constData();
	auto windows = uiApplication_->windows();
	auto findIt = windows.find( id );
	if (findIt == windows.end())
	{
		qWarning( "Failed to close window: Could not find window: %s \n", id.c_str() );
		return;
	}
	findIt->second->hide();
}

QMetaObject * QtScriptingEngine::getMetaObject( const IClassDefinition & classDefinition )
{
	auto definition = classDefinition.getName();

	{
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );
		auto metaObjectIt = metaObjects_.find( definition );
		if ( metaObjectIt != metaObjects_.end() )
		{
			return metaObjectIt->second;
		}
	}

	QMetaObjectBuilder builder;
	builder.setClassName( definition );
	builder.setSuperClass( &QObject::staticMetaObject );

	auto thisProperty = builder.addProperty( "self", "QObject*" );
	thisProperty.setWritable( false );
	thisProperty.setConstant( true );

	// Add all the properties from the ClassDefinition to the builder
	auto properties = classDefinition.allProperties();
	auto it = properties.begin();
	for (; it != properties.end(); ++it)
	{
		if (it->isMethod())
		{
			continue;
		}

		auto property = builder.addProperty( it->getName(), "QVariant" );
		property.setWritable( true );

		auto notifySignal = std::string( it->getName() ) + "Changed(QVariant)";
		property.setNotifySignal( builder.addSignal( notifySignal.c_str() ) );
	}

	std::vector<std::pair<std::string, std::string>> methodSignatures;
	std::string methodSignature;

	// TODO: Move these three to actual methods on the scripting engine.
	methodSignatures.emplace_back( "getMetaObject(QString)", "QVariant" );
	methodSignatures.emplace_back( "getMetaObject(QString,QString)", "QVariant" );
	methodSignatures.emplace_back( "containsMetaType(QString,QString)", "QVariant" );

	for (it = properties.begin(); it != properties.end(); ++it)
	{
		if (!it->isMethod())
		{
			continue;
		}

		methodSignature = it->getName();
		methodSignature += "(";

		for (size_t i = 0; i < it->parameterCount(); ++i)
		{
			methodSignature += "QVariant";

			if (i < it->parameterCount() - 1)
			{
				methodSignature += ",";
			}
		}

		methodSignature += ")";

		// TODO - determine if the function does not have a return type.
		// currently 'invoke' will always return a Variant regardless
		methodSignatures.emplace_back( std::move( methodSignature ), "QVariant" );
	}

	// skip index 0 as it has the same name as the one at index 1.
	for (size_t i = 1; i < methodSignatures.size(); ++i)
	{
		methodSignature = methodSignatures[i].first.substr( 0, methodSignatures[i].first.find( '(' ) ) + "Invoked()";
		builder.addSignal( methodSignature.c_str() );
	}

	for (size_t i = 0; i < methodSignatures.size(); ++i)
	{
		QMetaMethodBuilder method = builder.addMethod(
			methodSignatures[i].first.c_str(),
			methodSignatures[i].second.c_str() );
	}

	auto metaObject = builder.toMetaObject();
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	{
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );
		auto inserted = metaObjects_.insert( 
			std::pair< std::string, QMetaObject * >( definition, metaObject ) );
		if (!inserted.second)
		{
			free( metaObject );
		}
		return inserted.first->second;
	}
}