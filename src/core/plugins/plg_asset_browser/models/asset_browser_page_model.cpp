#include "asset_browser_page_model.hpp"
#include "folder_tree_model.hpp"
#include "folder_content_object_model.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "asset_browser/i_asset_listener.hpp"

struct AssetBrowserPageModel::Implementation
{
	Implementation( AssetBrowserPageModel& self );

	AssetBrowserPageModel& self_;
	IDefinitionManager* definitionManager_;
	GenericList folderContents_;
	GenericList breadcrumbs_;
	std::vector<IAssetListener*> listeners_;
	std::shared_ptr<ITreeModel> folders_;
	int currentSelectedAssetIndex_;

	void generateBreadcrumbs();
	void addBreadcrumb( const char* value );
};

AssetBrowserPageModel::Implementation::Implementation( 
	AssetBrowserPageModel& self )
	: self_( self )
	, definitionManager_( nullptr )
	, folders_( new FolderTreeModel() )
	, currentSelectedAssetIndex_( -1 )
{
}

void AssetBrowserPageModel::Implementation::generateBreadcrumbs()
{
	breadcrumbs_.clear();

	// TODO: Developers will need to provide their own filtering and
	//       generation of breadcrumbs in their implementations of the
	//       page model.
	addBreadcrumb( "C" );
	addBreadcrumb( "Objects" );
	addBreadcrumb( "Assets" );
}

void AssetBrowserPageModel::Implementation::addBreadcrumb( const char* value )
{
	breadcrumbs_.push_back( value );
}

AssetBrowserPageModel::AssetBrowserPageModel()
	: impl_( new Implementation( *this ) )
{
	int i = 0; 
	++i;
}

AssetBrowserPageModel::AssetBrowserPageModel( 
	const AssetBrowserPageModel& rhs )
	: impl_( new Implementation( *this ) )
{
	impl_->definitionManager_ = rhs.impl_->definitionManager_;
}

AssetBrowserPageModel::~AssetBrowserPageModel()
{
}

void AssetBrowserPageModel::init( IContextManager& contextManager )
{
	//TODO: Require the asset file location to be provided. IFileSystem
	//      will be used to handle everything from here as far as
	//      tree population goes.

	impl_->definitionManager_ =
		contextManager.queryInterface<IDefinitionManager>();
	assert( impl_->definitionManager_ != nullptr );

	//TODO: Remove this when we are done iterating on design and
	//      using proper population of folder contents based on folder
	//      selection.
	populateFolderContents( "fakepath_replace_later" );
}

void AssetBrowserPageModel::addListener( IAssetListener* listener )
{
	if (listener == nullptr)
	{
		return;
	}

	impl_->listeners_.push_back( listener );
}

void AssetBrowserPageModel::populateFolderContents( 
	const char* assetFolderPath )
{
	//TODO: Fetch contents and populate the list based on files that
	//      are in the provided asset folder path.

	impl_->folderContents_.clear();

	// Test code only while undergoing design iteration
	addFolderItem( "Object_01", "thumbnail.png" );
	addFolderItem( "Object_02", "thumbnail.png" );
	addFolderItem( "Object_03", "thumbnail.png" );
	addFolderItem( "Object_04", "thumbnail.png" );
}

void AssetBrowserPageModel::addFolderItem( const char* fileName, 
										   const char* thumbnail )
{
	ObjectHandle object = 
		impl_->definitionManager_->create<FolderContentObjectModel>( false );

	object.getBase<FolderContentObjectModel>()->init( fileName, thumbnail );

	impl_->folderContents_.push_back( object );
}

ObjectHandle AssetBrowserPageModel::getBreadcrumbs() const
{
	impl_->generateBreadcrumbs();

	return impl_->breadcrumbs_;
}

ObjectHandle AssetBrowserPageModel::getFolderContents() const
{
	return impl_->folderContents_;
}

ObjectHandle AssetBrowserPageModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
}

const int & AssetBrowserPageModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void AssetBrowserPageModel::currentSelectedAssetIndex( const int & index )
{
	impl_->currentSelectedAssetIndex_ = index;
}

ObjectHandle AssetBrowserPageModel::applyAsset() const
{
	if (impl_->currentSelectedAssetIndex_ > -1)
	{
		std::vector<IAssetListener*>::iterator itrListener = 
			impl_->listeners_.begin();
		std::vector<IAssetListener*>::iterator itrListenerEnd = 
			impl_->listeners_.end();

		for (; itrListener != itrListenerEnd; ++itrListener)
		{
			auto listener = (*itrListener);
			if (listener != nullptr)
			{
				// TODO: Pass a hardcoded model for now.
				listener->useAsset( "characters/npc/chicken/chicken.model" );
			}
		}
	}

	return nullptr;
}

ObjectHandle AssetBrowserPageModel::navigateHistoryForward() const
{
	// TODO: Just a hook for when the history is in place. The forward
	// and backward navigation buttons will call this function to
	// flip through the history. No history exists yet, hence the placeholder.
	return nullptr;
}

ObjectHandle AssetBrowserPageModel::navigateHistoryBackward() const
{
	// TODO: Just a hook for when the history is in place. The forward
	// and backward navigation buttons will call this function to
	// flip through the history. No history exists yet, hence the placeholder.
	return nullptr;
}
