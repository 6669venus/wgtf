#include "python_panel.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"


struct PythonObjects
{
	DECLARE_REFLECTED

	ObjectHandle oldStylePythonObject_;
	ObjectHandle newStylePythonObject_;
};


BEGIN_EXPOSE( PythonObjects, MetaNone() )
	EXPOSE( "OldStylePythonObject", oldStylePythonObject_, MetaNone() )
	EXPOSE( "NewStylePythonObject", newStylePythonObject_, MetaNone() )
END_EXPOSE()


PythonPanel::PythonPanel( IComponentContext& context )
	: Depends( context )
{
}


void PythonPanel::initialise()
{
	Variant::setMetaTypeManager( get<IMetaTypeManager>() );

	if (!this->createContextObject())
	{
		return;
	}

	if (!this->createPythonObjects())
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
	destroyPythonObjects();
}


bool PythonPanel::createContextObject()
{
	auto definitionManager = get<IDefinitionManager>();

	if (definitionManager == nullptr)
	{
		return false;
	}

	definitionManager->registerDefinition( new TypeClassDefinition<PythonObjects>() );
	contextObject_ = definitionManager->create<PythonObjects>();

	return true;
}


bool PythonPanel::createPythonObjects()
{
	auto scriptingEngine = get<IPythonScriptingEngine>();

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

	PythonObjects* pythonObjects = contextObject_.get();
	assert( pythonObjects != nullptr );

	auto definitionManager = get<IDefinitionManager>();
	auto moduleDefinition = module.getDefinition( *definitionManager );

	pythonObjects->oldStylePythonObject_ =
		ObjectHandle( moduleDefinition->bindProperty( "oldStyleObject", module ).getValue() );
	pythonObjects->newStylePythonObject_ =
		ObjectHandle( moduleDefinition->bindProperty( "newStyleObject", module ).getValue() );

	return true;
}


bool PythonPanel::addPanel()
{
	auto uiFramework = get<IUIFramework>();
	auto uiApplication = get<IUIApplication>();
	auto definitionManager = get<IDefinitionManager>(); 
	auto controller = get<IReflectionController>();
	
	if (uiFramework == nullptr || uiApplication == nullptr || definitionManager == nullptr || controller == nullptr)
	{
		return false;
	}

	std::unique_ptr<ITreeModel> model( new ReflectedTreeModel( contextObject_, *definitionManager, controller ) );

	pythonView_ = uiFramework->createView(
		"plg_python27_ui_test/python_object_test_panel.qml", IUIFramework::ResourceType::Url, std::move( model ) );

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


void PythonPanel::destroyPythonObjects()
{
	PythonObjects* pythonObjects = contextObject_.get();
	assert( pythonObjects != nullptr );

	pythonObjects->oldStylePythonObject_ = nullptr;
	pythonObjects->newStylePythonObject_ = nullptr;
}
