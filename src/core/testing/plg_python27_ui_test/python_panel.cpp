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


struct PythonObjects
{
	DECLARE_REFLECTED
	Variant oldStylePythonObject_;
	Variant newStylePythonObject_;
};


BEGIN_EXPOSE( PythonObjects, MetaNone() )
	EXPOSE( "oldStylePythonObject", oldStylePythonObject_, MetaNoSerialization() )
	//TODO: This needs types of new class style Python objects, NGT-1555 needs to be done first.
	//EXPOSE( "newStylePythonObject", newStylePythonObject_, MetaNoSerialization() )
END_EXPOSE()


class PythonContextObject
{
public:
	bool initialise( IComponentContext& context );


	void finalise( ObjectHandle handle );


	void updateValues();


	ITreeModel* getTreeModel() const;


private:
	DECLARE_REFLECTED

	
	bool createPythonObjects( IDefinitionManager& definitionManager );


	bool createTreeModel( IDefinitionManager& definitionManager );


	void callMethod( Variant& object, IDefinitionManager& definitionManager, const std::string& name );


	void destroyTreeModel( IDefinitionManager& definitionManager, ObjectHandle handle );


	IComponentContext* context_;
	ObjectHandleT<PythonObjects> pythonObjects_;
	ITreeModel* treeModel_;
};


BEGIN_EXPOSE( PythonContextObject, MetaNone() )
	EXPOSE( "pythonObjects", getTreeModel, MetaNoSerialization() )
	EXPOSE_METHOD( "updateValues", updateValues )
END_EXPOSE()


bool PythonContextObject::initialise( IComponentContext& context )
{
	context_ = &context;
	treeModel_ = nullptr;
	
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


void PythonContextObject::finalise( ObjectHandle handle )
{
	pythonObjects_->oldStylePythonObject_ = Variant();
	pythonObjects_->newStylePythonObject_ = Variant();

	auto definitionManager = context_->queryInterface<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return;
	}

	destroyTreeModel( *definitionManager, handle );
}


void PythonContextObject::updateValues()
{
	auto definitionManager = context_->queryInterface<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return;
	}

	std::string methodName = "updateValues";
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


void PythonContextObject::callMethod( Variant& object, IDefinitionManager& definitionManager, const std::string& name )
{
	ObjectHandle handle = object.cast<ObjectHandle>();

	if (!handle.isValid())
	{
		return;
	}

	auto definition = handle.getDefinition( definitionManager );
	auto property = definition->findProperty( name.c_str() );

	ReflectedMethodParameters parameters;
	property->invoke( handle, parameters );
}


void PythonContextObject::destroyTreeModel( IDefinitionManager& definitionManager, ObjectHandle handle )
{
	auto definition = handle.getDefinition( definitionManager );
	PropertyAccessor accessor = definition->bindProperty( "pythonObjects", handle );
	ITreeModel* oldTreeModel = treeModel_;
	ITreeModel* nullTreeModel = nullptr;
	ObjectHandle nullTreeHandle = nullTreeModel;

	auto& listeners = definitionManager.getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	for (auto it = itBegin; it != itEnd; ++it)
	{
		it->get()->preSetValue( accessor, nullTreeHandle );
	}

	treeModel_ = nullTreeModel;

	for(auto it = itBegin; it != itEnd; ++it)
	{
		it->get()->postSetValue( accessor, nullTreeHandle );
	}

	delete oldTreeModel;
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
	contextObject_->finalise( contextObject_ );
}


bool PythonPanel::createContextObject()
{
	auto definitionManager = get<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return false;
	}

	const bool managed = true;
	definitionManager->registerDefinition( new TypeClassDefinition<PythonObjects>() );
	definitionManager->registerDefinition( new TypeClassDefinition<PythonContextObject>() );
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
