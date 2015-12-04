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
	Variant oldStylePythonObject_;
	Variant newStylePythonObject_;
};


// Reflected definition for the PythonObjects.
BEGIN_EXPOSE( PythonObjects, MetaNone() )
	EXPOSE( "oldStylePythonObject", oldStylePythonObject_, MetaNoSerialization() )
	//TODO: This needs types of new class style Python objects, NGT-1555 needs to be done first.
	//EXPOSE( "newStylePythonObject", newStylePythonObject_, MetaNoSerialization() )
END_EXPOSE()


// Context object for reference from QML.
class PythonContextObject
{
public:
	PythonContextObject();


	// Calls different initialisation steps.
	bool initialise( IComponentContext& context );


	// Calls different finalisation steps.
	void finalise();


	// Calls into the updateValues Python method for the Python objects.
	void updateValues();


	// Access to the Python objects in the form of a reflected tree.
	ITreeModel* getTreeModel() const;


private:
	DECLARE_REFLECTED

	
	// Uses the Python scripting engine to import a module and get hold of references to Python objects.
	bool createPythonObjects( IDefinitionManager& definitionManager );


	// Create a reflected tree containing the Python objects.
	bool createTreeModel( IDefinitionManager& definitionManager );


	// Call a method on a reflected Python object.
	void callMethod( Variant& object, IDefinitionManager& definitionManager, const char* name );


	IComponentContext* context_;
	ObjectHandleT<PythonObjects> pythonObjects_;
	ITreeModel* treeModel_;
};


// Reflected definition for PythonContextObject.
BEGIN_EXPOSE( PythonContextObject, MetaNone() )
	EXPOSE( "pythonObjects", getTreeModel, MetaNoSerialization() )
	EXPOSE_METHOD( "updateValues", updateValues )
END_EXPOSE()


PythonContextObject::PythonContextObject()
	: treeModel_( nullptr )
{
}


bool PythonContextObject::initialise( IComponentContext& context )
{
	context_ = &context;
	auto definitionManager = context_->queryInterface<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return false;
	}

	if (!createPythonObjects( *definitionManager ))
	{
		return false;
	}

	if (!createTreeModel( *definitionManager ))
	{
		return false;
	}

	return true;
}


void PythonContextObject::finalise()
{
	// Release Python references.
	pythonObjects_->oldStylePythonObject_ = Variant();
	pythonObjects_->newStylePythonObject_ = Variant();
	delete treeModel_;
}


void PythonContextObject::updateValues()
{
	auto definitionManager = context_->queryInterface<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return;
	}

	const char* methodName = "updateValues";
	callMethod(	pythonObjects_->oldStylePythonObject_, *definitionManager, methodName );
	//callMethod( pythonObjects_->newStylePythonObject_, *definitionManager, methodName );
}


ITreeModel* PythonContextObject::getTreeModel() const
{
	return treeModel_;
}


bool PythonContextObject::createPythonObjects( IDefinitionManager& definitionManager )
{
	auto scriptingEngine = context_->queryInterface<IPythonScriptingEngine>();

	if (scriptingEngine == nullptr)
	{
		return false;
	}

	if (!scriptingEngine->appendPath( L"..\\..\\..\\src\\core\\testing\\plg_python27_ui_test\\scripts" ))
	{
		return false;
	}

	ObjectHandle module = scriptingEngine->import( "test_objects" );

	if (!module.isValid())
	{
		return false;
	}

	const bool managed = true;
	pythonObjects_ = definitionManager.create<PythonObjects>( !managed );
	auto moduleDefinition = module.getDefinition( definitionManager );

	auto property = moduleDefinition->findProperty( "oldStyleObject" );
	pythonObjects_->oldStylePythonObject_ = property->get( module, definitionManager );

	//property = moduleDefinition->findProperty( "newStyleObject" );
	//pythonObjects_->newStylePythonObject_ = property->get( module, definitionManager );
	
	return true;
}


bool PythonContextObject::createTreeModel( IDefinitionManager& definitionManager )
{
	auto controller = context_->queryInterface<IReflectionController>();

	if (controller == nullptr)
	{
		return false;
	}

	treeModel_ = new ReflectedTreeModel( pythonObjects_, definitionManager, controller );
	return true;
}


void PythonContextObject::callMethod( Variant& object, IDefinitionManager& definitionManager, const char* name )
{
	ObjectHandle handle = object.cast<ObjectHandle>();

	if (!handle.isValid())
	{
		return;
	}

	auto definition = handle.getDefinition( definitionManager );
	auto property = definition->findProperty( name );

	ReflectedMethodParameters parameters;
	property->invoke( handle, parameters );
}


PythonPanel::PythonPanel( IComponentContext& context )
	: Depends( context )
	, context_( context )
{
}


void PythonPanel::initialise()
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


void PythonPanel::finalise()
{
	removePanel();
	contextObject_->finalise();
}


bool PythonPanel::createContextObject()
{
	auto definitionManager = get<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return false;
	}

	definitionManager->registerDefinition( new TypeClassDefinition<PythonObjects>() );
	definitionManager->registerDefinition( new TypeClassDefinition<PythonContextObject>() );

	const bool managed = true;
	contextObject_ = definitionManager->create<PythonContextObject>( managed );

	if (!contextObject_->initialise( context_ ))
	{
		return false;
	}

	return true;
}


bool PythonPanel::addPanel()
{
	auto uiFramework = get<IUIFramework>();
	auto uiApplication = get<IUIApplication>();
	
	if (uiFramework == nullptr || uiApplication == nullptr)
	{
		return false;
	}

	pythonView_ = uiFramework->createView(
		"plg_python27_ui_test/python_object_test_panel.qml", IUIFramework::ResourceType::Url, contextObject_ );

	uiApplication->addView( *pythonView_ );
	return true;
}


void PythonPanel::removePanel()
{
	auto uiApplication = get<IUIApplication>();
	
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *pythonView_ );
}
