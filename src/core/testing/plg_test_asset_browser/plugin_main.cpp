#include "test_asset_browser_manager.hpp"
#include "filters/asset_browser_list_custom_filter.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "models/test_asset_browser_models.hpp"
#include "data_model/asset_browser/folder_content_object_model.hpp"
#include "qt_common/shared_controls.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/type_class_definition.hpp"
#include "variant/variant.hpp"
#include <vector>

class TestAssetBrowserPlugin
	: public PluginMain
{
public:

	TestAssetBrowserPlugin( IContextManager & contextManager )
		: assetBrowserManager_( nullptr )
		, listCustomFilter_( nullptr )
	{
	}

	bool PostLoad( IContextManager & contextManager ) override
	{		
		auto definitionManager = 
			contextManager.queryInterface<IDefinitionManager>();
		assert( definitionManager != nullptr );
		
		assetBrowserManager_ = new TestAssetBrowserManager( contextManager );

		// Custom checkFilter function for the AssetBrowserListFilter
		listCustomFilter_ = new AssetBrowserListCustomFilter();

		// Let the context manager handle the lifetime of these instances
		types_.push_back( contextManager.registerInterface( 
			assetBrowserManager_ ) );

		types_.push_back( contextManager.registerInterface( 
			listCustomFilter_ ) );

		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{		
 		Variant::setMetaTypeManager( 
 			contextManager.queryInterface< IMetaTypeManager >() );

		assetBrowserManager_->initialise( contextManager );
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}

private:

	std::vector< IInterface * > types_;
	TestAssetBrowserManager* assetBrowserManager_;
	AssetBrowserListCustomFilter * listCustomFilter_;
};

PLG_CALLBACK_FUNC( TestAssetBrowserPlugin )