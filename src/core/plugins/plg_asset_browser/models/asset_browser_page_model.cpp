#include "asset_browser_page_model.hpp"
#include "folder_tree_model.hpp"
#include "folder_content_object_model.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "data_model/value_change_notifier.hpp"
#include "asset_browser/i_asset_listener.hpp"
#include "serialization/interfaces/i_file_system.hpp"
#include "logging/logging.hpp"
#include "folder_tree_item.hpp"

#include <string>

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
	IFileSystem* fileSystem_;
	std::vector<std::string> assetPaths_;
	std::vector<ITreeModel::ItemIndex> foldersCrumb_;
	ValueChangeNotifier< size_t > currentBreadcrumbItemIndex_;

	void addFolderItem( const FileInfo& fileInfo );

	void generateBreadcrumbs();
	void addBreadcrumb( const char* value );
	static const size_t MAX_FOLDER_CONTENTS = 200;
	static const size_t NO_SELECTION = -1;
};

AssetBrowserPageModel::Implementation::Implementation( 
	AssetBrowserPageModel& self )
	: self_( self )
	, definitionManager_( nullptr )
	, folders_( nullptr )
	, currentSelectedAssetIndex_( -1 )
	, fileSystem_()
	, currentBreadcrumbItemIndex_( NO_SELECTION )
{
}

void AssetBrowserPageModel::Implementation::generateBreadcrumbs()
{
	breadcrumbs_.clear();

	// TODO: Developers will need to provide their own filtering and
	//       generation of breadcrumbs in their implementations of the
	//       page model.
}

void AssetBrowserPageModel::Implementation::addBreadcrumb( const char* value )
{
	breadcrumbs_.push_back( value );
}


void AssetBrowserPageModel::Implementation::addFolderItem(
	const FileInfo& fileInfo )
{
	if (folderContents_.size() >= MAX_FOLDER_CONTENTS)
	{
		return;
	}
	
	ObjectHandle object =
		definitionManager_->create<FolderContentObjectModel>( false );

	object.getBase<FolderContentObjectModel>()->init( fileInfo );

	folderContents_.push_back( object );
}

AssetBrowserPageModel::AssetBrowserPageModel()
	: impl_( new Implementation( *this ) )
{
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
	impl_->definitionManager_ =
		contextManager.queryInterface<IDefinitionManager>();
	assert( impl_->definitionManager_ != nullptr );

	impl_->fileSystem_ = contextManager.queryInterface<IFileSystem>();
	assert( impl_->fileSystem_ != nullptr );
	
	//TODO: Require the asset file location to be provided.
	addAssetPath( "../../../../../game/res/bigworld" );

	impl_->folders_.reset( new FolderTreeModel( *this ) );
}

void AssetBrowserPageModel::addListener( IAssetListener* listener )
{
	if (listener == nullptr)
	{
		return;
	}

	impl_->listeners_.push_back( listener );
}


const std::vector<std::string>& AssetBrowserPageModel::assetPaths() const
{
	return impl_->assetPaths_;
}

bool AssetBrowserPageModel::addAssetPath(const std::string& path)
{
	if (std::find(impl_->assetPaths_.begin(), impl_->assetPaths_.end(), path)
			!= impl_->assetPaths_.end())
	{
		return true;
	}

	if (!impl_->fileSystem_->exists( path.c_str() ))
	{
		NGT_ERROR_MSG( "AssetBrowserPageModel::populateFolderContents: "
		"assert folder path is not exists: %s\n", path.c_str() );
		return false;
	}

	impl_->assetPaths_.push_back( path );

	return true;
}


IFileSystem* AssetBrowserPageModel::fileSystem() const
{
	return impl_->fileSystem_;
}

void AssetBrowserPageModel::addFolderItems( 
	const std::vector<std::string>& paths)
{
	IFileSystem * fs = impl_->fileSystem_;

	std::list<std::string> directories;
	
	for (auto& path : paths)
	{
		if (!fs->exists( path.c_str() ))
		{
			NGT_WARNING_MSG( "AssetBrowserPageModel::populateFolderContents: "
				"assert folder path is not exists: %s\n", path.c_str() );
			continue;
		}

		directories.push_back( path );
	}

	while (!directories.empty())
	{
		const std::string& dir = directories.front();

		fs->enumerate( dir.c_str(), [&](FileInfo&& info) {
			if (!info.isDirectory())
			{
				impl_->addFolderItem( info );
			}
			else if (!info.isDots())
			{
				directories.push_back( info.fullPath );
			}
			return true;
		});

		directories.pop_front();
	}
}

void AssetBrowserPageModel::populateFolderContents( 
	const std::vector<std::string>& paths )
{
	impl_->folderContents_.clear();
	addFolderItems( paths );
}


ObjectHandle AssetBrowserPageModel::getBreadcrumbs() const
{
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

const char* AssetBrowserPageModel::getThumbnail( const char * filename ) const
{
	//TODO
	return "thumbnail.png";
}

const int & AssetBrowserPageModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void AssetBrowserPageModel::currentSelectedAssetIndex( const int & index )
{
	impl_->currentSelectedAssetIndex_ = index;
}

bool AssetBrowserPageModel::applyAsset() const
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

	return true;
}

/// Navigate forward
bool AssetBrowserPageModel::navigateHistoryForward() const
{
	if (impl_->foldersCrumb_.size() > impl_->currentBreadcrumbItemIndex_.value() + 1)
	{
		// Update the current breadcrumb item index and let the listeners know
		// the data has been changed.
		impl_->currentBreadcrumbItemIndex_.notifyPostDataChanged();
		size_t currentValue = impl_->currentBreadcrumbItemIndex_.value();
		impl_->currentBreadcrumbItemIndex_.value( currentValue + 1 );
		impl_->currentBreadcrumbItemIndex_.notifyPreDataChanged();
	}

	return true;
}

/// Navigate backward
bool AssetBrowserPageModel::navigateHistoryBackward() const
{
	// Update the current breadcrumb item index and let the listeners know
	// the data has been changed.
	if (0 < impl_->currentBreadcrumbItemIndex_.value())
	{
		impl_->currentBreadcrumbItemIndex_.notifyPostDataChanged();
		size_t currentValue = impl_->currentBreadcrumbItemIndex_.value();
		impl_->currentBreadcrumbItemIndex_.value( currentValue - 1 );
		impl_->currentBreadcrumbItemIndex_.notifyPreDataChanged();
	}

	return true;
}

Variant AssetBrowserPageModel::getFolderTreeItemSelected() const
{
	return Variant();
}

void AssetBrowserPageModel::setFolderTreeItemSelected( const Variant& selectedItem )
{
	auto item = reinterpret_cast< const FolderTreeItem *>( selectedItem.value<intptr_t>());
	
	if ( item )
	{
		const FileInfo& fileInfo = item->getFileInfo();
		std::vector< std::string > paths;
		paths.push_back( fileInfo.fullPath );

		std::string token = "\\";
		auto lastToken = std::find_end( fileInfo.fullPath.begin(), fileInfo.fullPath.end(), token.begin(), token.end() );

		// Directory type only
		if (impl_->folders_ && fileInfo.isDirectory())
		{
			ITreeModel::ItemIndex selectedItemIndex = impl_->folders_->index( item );
			auto foundItemIndex = std::find( impl_->foldersCrumb_.begin(), impl_->foldersCrumb_.end(), selectedItemIndex );
			
			// Don't add same ItemIndex twice
			if ( impl_->foldersCrumb_.end() == foundItemIndex )
			{
				impl_->foldersCrumb_.push_back( selectedItemIndex );
				impl_->currentBreadcrumbItemIndex_.value( impl_->foldersCrumb_.size() - 1 );

				if (fileInfo.fullPath.end() != lastToken)
				{
					// Just grab the token and the folder name.
					// e.g. "\models"
					std::string crumb = "";
					crumb.append( lastToken, fileInfo.fullPath.end() );
					impl_->addBreadcrumb( crumb.c_str() );
				}
			}
		}

		this->populateFolderContents( paths );
	}
}

ObjectHandle AssetBrowserPageModel::currentBreadcrumbItemIndex() const
{
	return ObjectHandle( &impl_->currentBreadcrumbItemIndex_ );
}

const size_t & AssetBrowserPageModel::getCurrentBreadcrumbItemIndex() const
{
	return impl_->currentBreadcrumbItemIndex_.value();
}

void AssetBrowserPageModel::setCurrentBreadcrumbItemIndex( const size_t & index )
{
	impl_->currentBreadcrumbItemIndex_.value( index );
}
