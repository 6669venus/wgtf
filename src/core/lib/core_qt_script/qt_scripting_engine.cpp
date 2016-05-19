#include "qt_scripting_engine.hpp"

#include "qobject_qt_type_converter.hpp"
#include "qt_script_object.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/controls/bw_copyable.hpp"
#include "script_qt_type_converter.hpp"
#include "wg_list_iterator.hpp"
#include "collection_qt_type_converter.hpp"
#include "core_qt_common/image_qt_type_converter.hpp"
#include "core_qt_common/model_qt_type_converter.hpp"

#include "core_reflection/base_property.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_command_system/i_command_manager.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_data_model/i_list_model.hpp"

#include <private/qmetaobjectbuilder_p.h>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPointer>

namespace wgt
{
Q_DECLARE_METATYPE( ObjectHandle );

typedef std::vector< QPointer<QtScriptObject> > ScriptObjects;

struct QtScriptingEngine::Implementation
{
	Implementation( QtScriptingEngine& self )
		: self_( self )
		, defManager_( nullptr )
		, commandSystemProvider_( nullptr )
		, copyPasteManager_( nullptr )
		, uiApplication_( nullptr )
		, uiFramework_( nullptr )
		, contextManager_( nullptr )
	{
		propListener_ = std::make_shared<PropertyListener>( scriptObjects_ );
	}

	~Implementation()
	{
		propListener_ = nullptr;
		assert( scriptObjects_.empty() );
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );

		for (auto& metaObjectPair: metaObjects_)
		{
			free( metaObjectPair.second );
		}

		metaObjects_.clear();
	}

	void initialise( IQtFramework& qtFramework, IComponentContext& contextManager );

	QtScriptObject* createScriptObject( const ObjectHandle& object );
	QMetaObject* getMetaObject( const IClassDefinition& classDefinition );

	QtScriptingEngine& self_;

	IDefinitionManager* defManager_;
	ICommandManager* commandSystemProvider_;
	ICopyPasteManager* copyPasteManager_;
	IUIApplication* uiApplication_;
	IUIFramework* uiFramework_;
	IComponentContext* contextManager_;

	std::mutex metaObjectsMutex_;
	std::map<std::string, QMetaObject*> metaObjects_;
	std::vector<std::unique_ptr< IQtTypeConverter>> qtTypeConverters_;
	std::map<ObjectHandle, ScriptObjects > scriptObjects_;

	struct PropertyListener: public PropertyAccessorListener
	{
		PropertyListener( const std::map<ObjectHandle, ScriptObjects >& scriptObjects )
			: scriptObjects_( scriptObjects )
		{}

		void postSetValue( const PropertyAccessor& accessor, const Variant& value ) override;
		void postInvoke( const PropertyAccessor & accessor, Variant result, bool undo ) override;

		const std::map<ObjectHandle, ScriptObjects >& scriptObjects_;
	};

	std::shared_ptr<PropertyAccessorListener> propListener_;
};


void QtScriptingEngine::Implementation::initialise( IQtFramework& qtFramework, IComponentContext& contextManager )
{	
	contextManager_ = &contextManager;
	defManager_ = contextManager.queryInterface<IDefinitionManager>();
	commandSystemProvider_ = contextManager.queryInterface<ICommandManager>();
	copyPasteManager_ = contextManager.queryInterface<ICopyPasteManager>();
	uiApplication_ = contextManager_->queryInterface<IUIApplication>();
	uiFramework_ = contextManager_->queryInterface<IUIFramework>();
	assert( defManager_ );
	assert( commandSystemProvider_ );
	assert( copyPasteManager_ );
	assert( uiApplication_ );
	assert( uiFramework_ );

	// TODO: All but the scriptTypeConverter need to be moved to the qt app plugin.
	qtTypeConverters_.emplace_back( new GenericQtTypeConverter< ObjectHandle >() );
	qtTypeConverters_.emplace_back( new ImageQtTypeConverter() );
	qtTypeConverters_.emplace_back( new QObjectQtTypeConverter() );
	qtTypeConverters_.emplace_back( new ModelQtTypeConverter() );	
	qtTypeConverters_.emplace_back( new CollectionQtTypeConverter() );
	qtTypeConverters_.emplace_back( new ScriptQtTypeConverter( self_ ) );

	QMetaType::registerComparators<ObjectHandle>();

	for (auto& qtTypeConverter : qtTypeConverters_)
	{
		qtFramework.registerTypeConverter( *qtTypeConverter );
	}

	defManager_->registerPropertyAccessorListener( propListener_ );
}


void QtScriptingEngine::Implementation::PropertyListener::postSetValue(
	const PropertyAccessor& accessor, const Variant& value )
{
	const ObjectHandle& object = accessor.getObject();
	auto itr = scriptObjects_.find( object );

	if (itr == scriptObjects_.end())
	{
		return;
	}

	// Copy collection to accommodate re-entry
    auto scriptObjects = itr->second;
    for( auto scriptObject : scriptObjects )
    {
        assert( !scriptObject.isNull() );
        if(!scriptObject.isNull())
        {
	        scriptObject->firePropertySignal( accessor.getProperty(), value );
        }
    }
}


void QtScriptingEngine::Implementation::PropertyListener::postInvoke(
	const PropertyAccessor & accessor, Variant result, bool undo )
{
	const ObjectHandle& object = accessor.getObject();
	auto itr = scriptObjects_.find( object );

	if (itr == scriptObjects_.end())
	{
		return;
	}
    auto& scriptObjects = itr->second;
    for( auto scriptObject : scriptObjects )
    {
        assert( !scriptObject.isNull() );
        if(!scriptObject.isNull())
        {
	        scriptObject->fireMethodSignal( accessor.getProperty(), undo );
        }
    }
}


QtScriptObject* QtScriptingEngine::Implementation::createScriptObject( const ObjectHandle& object )
{
	if (!object.isValid())
	{
		return nullptr;
	}

	auto root = reflectedRoot( object, *defManager_ );

	auto classDefinition = root.getDefinition( *defManager_ );
	if (classDefinition == nullptr)
	{
		return nullptr;
	}

	auto metaObject = getMetaObject( *classDefinition );
	if (metaObject == nullptr)
	{
		return nullptr;
	}

    auto itr = scriptObjects_.find( root );

    if (itr == scriptObjects_.end())
    {
		itr = scriptObjects_.insert(std::make_pair(root, ScriptObjects())).first;
    }
    assert( itr != scriptObjects_.end() );
	auto& scriptObjects = itr->second;

	assert( contextManager_ );
	QtScriptObject* scriptObject = new QtScriptObject(
		*contextManager_, self_, *metaObject, root, nullptr );
    // always set scripteObject ownership to QML
    QQmlEngine::setObjectOwnership( scriptObject, QQmlEngine::JavaScriptOwnership );

	scriptObjects.emplace_back(scriptObject );
	return scriptObject;
}


QMetaObject* QtScriptingEngine::Implementation::getMetaObject( const IClassDefinition& classDefinition )
{
	auto definition = classDefinition.getName();

	{
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );
		const auto& metaObjectIt = metaObjects_.find( definition );
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
		property.setWritable( !it->readOnly() );

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
		methodSignature =
			methodSignatures[i].first.substr( 0, methodSignatures[i].first.find( '(' ) ) +
			"Invoked(QVariant)";
		QMetaMethodBuilder method = builder.addSignal( methodSignature.c_str() );
		QList<QByteArray> parameterNames;
		parameterNames.append( "undo" );
		method.setParameterNames( parameterNames );
	}

	for (size_t i = 0; i < methodSignatures.size(); ++i)
	{
		builder.addMethod( methodSignatures[i].first.c_str(), methodSignatures[i].second.c_str() );
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

QtScriptingEngine::QtScriptingEngine()
	: impl_( new Implementation( *this ) )
{
}

QtScriptingEngine::~QtScriptingEngine()
{
	impl_ = nullptr;
}

void QtScriptingEngine::initialise( IQtFramework & qtFramework, IComponentContext & contextManager )
{	
	impl_->initialise( qtFramework, contextManager );
}

void QtScriptingEngine::finalise()
{
    assert( impl_->scriptObjects_.empty() );
	while (!impl_->scriptObjects_.empty())
	{
        // normally, code should never goes here
        // if it goes here, we need to figure out why
        // QScriptObject not get destroyed correctly.
		auto iter = impl_->scriptObjects_.begin();
        auto& scriptObjects = iter->second;
        while (!scriptObjects.empty())
        {
            auto&& scriptObject = scriptObjects.begin();
            if(!scriptObject->isNull())
            {
                delete scriptObject->data();
            }
        }
	}

	impl_->scriptObjects_.clear();
}

void QtScriptingEngine::deregisterScriptObject( QtScriptObject & scriptObject )
{
	std::map<ObjectHandle, ScriptObjects>::iterator findIt = 
		impl_->scriptObjects_.find( scriptObject.object() );
	assert (findIt != impl_->scriptObjects_.end());
    auto&& scriptObjects = findIt->second;
    scriptObjects.erase( 
        std::remove( scriptObjects.begin(), scriptObjects.end(), &scriptObject), scriptObjects.end());
    if( scriptObjects.empty() )
    {
	    impl_->scriptObjects_.erase( findIt->first );
    }
}

QtScriptObject * QtScriptingEngine::createScriptObject( 
	const ObjectHandle & object )
{
	return impl_->createScriptObject( object );
}

QObject * QtScriptingEngine::createObject( QString definition )
{
	auto className = std::string( "class " ) + definition.toUtf8().constData();

	if (impl_->defManager_ == nullptr)
	{
		qCritical( "Definition manager not found. Could not create object: %s \n", 
			className.c_str() );
		return nullptr;
	}

	auto classDefinition = impl_->defManager_->getDefinition( className.c_str() );
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
	Command * cmd = impl_->commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Could not find Command: %s \n", commandId.c_str() );
		return false;
	}
	impl_->commandSystemProvider_->queueCommand( commandId.c_str() );
	return true;
}

void QtScriptingEngine::beginUndoFrame()
{
	impl_->commandSystemProvider_->beginBatchCommand();
}

void QtScriptingEngine::endUndoFrame()
{
	impl_->commandSystemProvider_->endBatchCommand();
}

void QtScriptingEngine::abortUndoFrame()
{
	impl_->commandSystemProvider_->abortBatchCommand();
}

void QtScriptingEngine::deleteMacro( QString command )
{
	std::string commandId = command.toUtf8().constData();
	Command * cmd = impl_->commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Delete macro failed: Could not find Macro: %s \n", commandId.c_str() );
		return;
	}
	impl_->commandSystemProvider_->deleteMacroByName( commandId.c_str() );
}

void QtScriptingEngine::selectControl( BWCopyable* control, bool append )
{
	impl_->copyPasteManager_->onSelect( control, append );
}

void QtScriptingEngine::deselectControl( BWCopyable* control, bool reset )
{
	impl_->copyPasteManager_->onDeselect( control, reset );
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
	auto windows = impl_->uiApplication_->windows();
	auto findIt = windows.find( id );
	if (findIt == windows.end())
	{
		qWarning( "Failed to close window: Could not find window: %s \n", id.c_str() );
		return;
	}
	findIt->second->close();
}

IDefinitionManager* QtScriptingEngine::getDefinitionManager()
{
	return impl_->defManager_;
}

void QtScriptingEngine::addPreference( const QString & preferenceId, const QString & propertyName, QVariant value )
{
	std::string id = preferenceId.toUtf8().constData();
	std::string name = propertyName.toUtf8().constData();
	std::string data = value.toString().toUtf8().constData();
	auto preference = impl_->uiFramework_->getPreferences()->getPreference( id.c_str() );
	preference->set( name.c_str(), data );
}

} // end namespace wgt
