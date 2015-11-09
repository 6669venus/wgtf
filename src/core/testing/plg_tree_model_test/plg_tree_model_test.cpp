#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "test_tree_model.hpp"
#include <vector>


//==============================================================================
class TreeModelTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
	std::unique_ptr<IView> treeView_;
public:
	//==========================================================================
	TreeModelTestPlugin(IComponentContext & contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );

		auto model = std::unique_ptr< ITreeModel >( new TestTreeModel() );
		treeView_ = uiFramework->createView( 
			"plg_tree_model_test/test_tree_panel.qml",
			IUIFramework::ResourceType::Url, std::move( model ) );

		uiApplication->addView( *treeView_ );
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeView( *treeView_ );
		treeView_ = nullptr;
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

};


PLG_CALLBACK_FUNC( TreeModelTestPlugin )

