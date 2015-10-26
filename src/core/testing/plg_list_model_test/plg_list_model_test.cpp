#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "test_list_model.hpp"
#include <vector>


//==============================================================================
class ListModelTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
	std::unique_ptr<IView> listView_;
	std::unique_ptr<IView> shortListView_;
public:
	//==========================================================================
	ListModelTestPlugin(IComponentContext & contextManager )
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

		std::unique_ptr< IListModel > listModel( new TestListModel() );
		listView_ = uiFramework->createView(
			"listmodel_testing/test_list_panel.qml",
			IUIFramework::ResourceType::Url, std::move( listModel ) );

		std::unique_ptr< IListModel > shortListModel( new TestListModel( true ) );
		shortListView_ = uiFramework->createView(
			"listmodel_testing/test_list_panel.qml",
			IUIFramework::ResourceType::Url, std::move( shortListModel ) );

		uiApplication->addView( *listView_ );
		uiApplication->addView( *shortListView_ );

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeView( *shortListView_ );
		uiApplication->removeView( *listView_ );
		listView_ = nullptr;
		shortListView_ = nullptr;
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


PLG_CALLBACK_FUNC( ListModelTestPlugin )

