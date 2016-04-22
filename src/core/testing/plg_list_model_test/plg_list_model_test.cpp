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
	std::unique_ptr<IView> oldListView_;
	std::unique_ptr<IView> oldShortListView_;
	std::unique_ptr<IView> listView_;
	std::unique_ptr<IView> shortListView_;
	std::unique_ptr<IView> multiColumnListView_;

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
		
		std::unique_ptr< IListModel > oldListModel( new OldTestListModel() );
		oldListView_ = uiFramework->createView(
			"plg_list_model_test/test_list_panel_old.qml",
			IUIFramework::ResourceType::Url, std::move( oldListModel ) );
		
		std::unique_ptr< IListModel > oldShortListModel( new OldTestListModel( true ) );
		oldShortListView_ = uiFramework->createView(
			"plg_list_model_test/test_short_list_panel_old.qml",
			IUIFramework::ResourceType::Url, std::move( oldShortListModel ) );
		
		std::unique_ptr<AbstractListModel> listModel( new TestListModel() );
		listView_ = uiFramework->createView(
			"plg_list_model_test/test_list_panel.qml",
			IUIFramework::ResourceType::Url, std::move( listModel ) );
		/*
		std::unique_ptr<AbstractListModel> shortListModel( new TestListModel( true ) );
		shortListView_ = uiFramework->createView(
			"plg_list_model_test/test_list_panel.qml",
			IUIFramework::ResourceType::Url, std::move( shortListModel ) );
		

		std::unique_ptr< IListModel > multiColumnListModel( new TestListModel( true ) );
		multiColumnListView_ = uiFramework->createView(
			"plg_list_model_test/test_column_sequence_list_panel.qml",
			IUIFramework::ResourceType::Url, std::move( multiColumnListModel ) );
		
		*/

		if (oldListView_ != nullptr)
		{
			uiApplication->addView( *oldListView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
		if (oldShortListView_ != nullptr)
		{
			uiApplication->addView( *oldShortListView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
		if (listView_ != nullptr)
		{
			uiApplication->addView( *listView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
		/*if (shortListView_ != nullptr)
		{
			uiApplication->addView( *shortListView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
		if (multiColumnListView_ != nullptr)
		{
			uiApplication->addView( *multiColumnListView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}*/

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		/*if (multiColumnListView_ != nullptr)
		{
			uiApplication->removeView( *multiColumnListView_ );
		}
		if (shortListView_ != nullptr)
		{
			uiApplication->removeView( *shortListView_ );
		}*/
		if (listView_ != nullptr)
		{
			uiApplication->removeView( *listView_ );
		}
		if (oldShortListView_ != nullptr)
		{
			uiApplication->removeView( *oldShortListView_ );
		}
		if (oldListView_ != nullptr)
		{
			uiApplication->removeView( *oldListView_ );
		}
		listView_ = nullptr;
		shortListView_ = nullptr;
		multiColumnListView_ = nullptr;
		oldListView_ = nullptr;
		oldShortListView_ = nullptr;
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

