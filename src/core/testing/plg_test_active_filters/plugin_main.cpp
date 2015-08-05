#include "generic_plugin/generic_plugin.hpp"
#include "variant/variant.hpp"
#include "variant/default_meta_type_manager.hpp"
#include "generic_plugin/interfaces/i_component_context.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_view.hpp"

#include "reflection/reflected_object.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/function_property.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"

#include "active_filters_test_view_model.hpp"

//------------------------------------------------------------------------------
// Plugin Class
// Spins up an instance of the plugin and creates the panel with the view
// model being passed is as source.
//------------------------------------------------------------------------------
class TestActiveFiltersPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestActiveFiltersPlugin( IComponentContext & contextManager ) {}

	//==========================================================================
	void Initialise(IComponentContext & contextManager) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		auto defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == nullptr)
		{
			return;
		}

		defManager->registerDefinition( new TypeClassDefinition< 
			ActiveFiltersTestViewModel >() );

		auto testViewModel = defManager->createT< 
			ActiveFiltersTestViewModel >();
		testViewModel->init( *defManager );

		auto qtFramework = contextManager.queryInterface< IQtFramework >();
		if (qtFramework == nullptr)
		{
			return;
		}

		testView_ = qtFramework->createView( 
			"qrc:///testing/active_filters_test_panel.qml", 
			IUIFramework::ResourceType::Url, testViewModel );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		uiApplication->addView( *testView_ );
	}

private:
	std::unique_ptr< IView > testView_;
};

PLG_CALLBACK_FUNC( TestActiveFiltersPlugin )

