#include "python_panel.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"


// Class to keep references to Python classes and to expose as a Reflected object.
struct PythonObjects
{
	DECLARE_REFLECTED
	Variant rootPythonObject_;
};


// Reflected definition for the PythonObjects.
BEGIN_EXPOSE( PythonObjects, MetaNone() )
	EXPOSE( "rootPythonObject", rootPythonObject_, MetaNoSerialization() )
END_EXPOSE()


// Context object for reference from QML.
class PythonContextObject
{
public:
	PythonContextObject();


	// Calls different initialization steps.
	bool initialize( IComponentContext & context );


	// Calls different finalization steps.
	void finalize( ObjectHandleT< PythonContextObject > & handle );


	// Access to the Python objects in the form of a reflected tree.
	ITreeModel * getTreeModel() const;


private:
	DECLARE_REFLECTED

	
	// Uses the Python scripting engine to import a module and get hold of
	// references to Python objects.
	bool createPythonObjects( IDefinitionManager & definitionManager );


	// Create a reflected tree containing the Python objects.
	bool createTreeModel( IDefinitionManager & definitionManager );


	// Call a method on a reflected Python object.
	void callMethod( Variant & object, IDefinitionManager & definitionManager,
		const char * name );


	// Clean up the tree.
	void destroyTreeModel( IDefinitionManager & definitionManager,
		ObjectHandleT< PythonContextObject > & handle );


	IComponentContext * context_;
	ObjectHandleT< PythonObjects > pythonObjects_;
	ITreeModel * treeModel_;
};


// Reflected definition for PythonContextObject.
BEGIN_EXPOSE( PythonContextObject, MetaNone() )
	EXPOSE( "pythonObjects", getTreeModel, MetaNoSerialization() )
END_EXPOSE()


PythonContextObject::PythonContextObject()
	: treeModel_( nullptr )
{
}


bool PythonContextObject::initialize( IComponentContext & context )
{
	context_ = &context;

	auto definitionManager = context_->queryInterface< IDefinitionManager >();
	if (definitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}

	if (!this->createPythonObjects( *definitionManager ))
	{
		return false;
	}

	if (!this->createTreeModel( *definitionManager ))
	{
		return false;
	}

	return true;
}


void PythonContextObject::finalize( ObjectHandleT< PythonContextObject > & handle )
{
	// Release Python references.
	pythonObjects_->rootPythonObject_ = Variant();

	auto definitionManager = context_->queryInterface< IDefinitionManager >();
	if (definitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return;
	}

	this->destroyTreeModel( *definitionManager, handle );
}


ITreeModel * PythonContextObject::getTreeModel() const
{
	return treeModel_;
}


bool PythonContextObject::createPythonObjects( IDefinitionManager & definitionManager )
{
	auto scriptingEngine = context_->queryInterface< IPythonScriptingEngine >();
	if (scriptingEngine == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IPythonScriptingEngine\n" );
		return false;
	}

	if (!scriptingEngine->appendPath(
		L"..\\..\\..\\src\\core\\third_party\\python\\Python-2.7.10\\Lib" ))
	{
		NGT_ERROR_MSG( "Failed to append path\n" );
		return false;
	}
	if (!scriptingEngine->appendPath(
		L"..\\..\\..\\src\\core\\testing\\plg_maps_settings\\scripts" ))
	{
		NGT_ERROR_MSG( "Failed to append path\n" );
		return false;
	}

	ObjectHandle module = scriptingEngine->import( "MapsSettingsEditor" );
	if (!module.isValid())
	{
		NGT_ERROR_MSG( "Could not import module\n" );
		return false;
	}

	const bool managed = true;
	pythonObjects_ = definitionManager.create< PythonObjects >( !managed );
	auto moduleDefinition = module.getDefinition( definitionManager );

	auto pProperty = moduleDefinition->findProperty( "rootPythonObject" );
	if (pProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	pythonObjects_->rootPythonObject_ = pProperty->get( module, definitionManager );
	if (pythonObjects_->rootPythonObject_.isVoid())
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	return true;
}


bool PythonContextObject::createTreeModel( IDefinitionManager & definitionManager )
{
	auto controller = context_->queryInterface< IReflectionController >();
	if (controller == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IReflectionController\n" );
		return false;
	}

	treeModel_ = new ReflectedTreeModel( pythonObjects_, definitionManager, controller );
	return true;
}


void PythonContextObject::callMethod( Variant & object,
	IDefinitionManager & definitionManager,
	const char * name )
{
	ObjectHandle handle = object.cast< ObjectHandle >();

	if (!handle.isValid())
	{
		return;
	}

	auto definition = handle.getDefinition( definitionManager );
	auto property = definition->findProperty( name );

	ReflectedMethodParameters parameters;
	property->invoke( handle, parameters );
}


void PythonContextObject::destroyTreeModel(
	IDefinitionManager & definitionManager,
	ObjectHandleT< PythonContextObject > & handle )
{
	auto definition = handle.getDefinition( definitionManager );
	PropertyAccessor accessor = definition->bindProperty( "pythonObjects", handle );
	ITreeModel * oldTreeModel = treeModel_;
	ITreeModel * nullTreeModel = nullptr;
	ObjectHandle nullTreeHandle = nullTreeModel;

	auto & listeners = definitionManager.getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	for (auto it = itBegin; it != itEnd; ++it)
	{
		it->get()->preSetValue( accessor, nullTreeHandle );
	}

	treeModel_ = nullTreeModel;

	for (auto it = itBegin; it != itEnd; ++it)
	{
		it->get()->postSetValue( accessor, nullTreeHandle );
	}

	delete oldTreeModel;
}


PythonPanel::PythonPanel( IComponentContext & context )
	: Depends( context )
	, context_( context )
{
}


void PythonPanel::initialize()
{
	if (!this->createContextObject())
	{
		return;
	}

	if (!this->addPanel())
	{
		return;
	}
}


void PythonPanel::finalize()
{
	this->removePanel();
	contextObject_->finalize( contextObject_ );
}


bool PythonPanel::createContextObject()
{
	auto definitionManager = get<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return false;
	}

	definitionManager->registerDefinition( new TypeClassDefinition< PythonObjects >() );
	definitionManager->registerDefinition( new TypeClassDefinition< PythonContextObject >() );

	const bool managed = true;
	contextObject_ = definitionManager->create< PythonContextObject >( managed );

	if (!contextObject_->initialize( context_ ))
	{
		return false;
	}

	return true;
}


bool PythonPanel::addPanel()
{
	auto uiFramework = get< IUIFramework >();
	auto uiApplication = get< IUIApplication >();
	
	if (uiFramework == nullptr || uiApplication == nullptr)
	{
		NGT_ERROR_MSG( "Failed to load panel\n" );
		return false;
	}

	pythonView_ = uiFramework->createView(
		"MapsSettings/MapsSettingsPanel.qml",
		IUIFramework::ResourceType::Url, contextObject_ );

	uiApplication->addView( *pythonView_ );
	return true;
}


void PythonPanel::removePanel()
{
	auto uiApplication = get< IUIApplication >();
	
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *pythonView_ );
}
