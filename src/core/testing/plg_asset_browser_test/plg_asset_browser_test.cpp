#include "test_asset_browser_manager.hpp"
#include "filters/asset_browser_list_custom_filter.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "models/test_asset_browser_models.hpp"
#include "core_data_model/asset_browser/folder_content_object_model.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_variant/variant.hpp"
#include <vector>

class TestAssetBrowserPlugin
	: public PluginMain
{
public:

	TestAssetBrowserPlugin( IComponentContext & contextManager )
		: assetBrowserManager_( nullptr )
		, listCustomFilter_( nullptr )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
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

	void Initialise( IComponentContext & contextManager ) override
	{		
 		Variant::setMetaTypeManager( 
 			contextManager.queryInterface< IMetaTypeManager >() );

		assetBrowserManager_->initialise( contextManager );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
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
