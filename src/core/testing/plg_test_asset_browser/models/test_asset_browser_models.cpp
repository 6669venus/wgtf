#include "test_asset_browser_models.hpp"
#include "data_model/asset_browser/folder_tree_model.hpp"
#include "data_model/asset_browser/folder_content_object_model.hpp"
#include "data_model/asset_browser/folder_tree_item.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "data_model/value_change_notifier.hpp"
#include "asset_browser/i_asset_listener.hpp"
#include "serialization/interfaces/i_file_system.hpp"
#include "reflection/i_definition_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "logging/logging.hpp"

#include <string>

//------------------------------------------------------------------------------

TestPageModel::TestPageModel()
{
}

TestPageModel::TestPageModel( const TestPageModel& rhs )
{
	testModel_ = rhs.testModel_;
}

TestPageModel::~TestPageModel()
{
}

void TestPageModel::init( IContextManager & contextManager )
{
	defManager_ = 
		contextManager.queryInterface<IDefinitionManager>();
	assert( defManager_ != nullptr );

	auto def = defManager_->getDefinition< IAssetBrowserModel >();
	auto impl = std::unique_ptr< IAssetBrowserModel >(
		new TestAssetBrowserModel );

	testModel_ = ObjectHandle( std::move( impl ), def );
	testModel_.getBase< IAssetBrowserModel >()->initialise( contextManager );
}

ObjectHandle TestPageModel::testModel() const
{
	return testModel_;
}

//------------------------------------------------------------------------------

struct TestAssetBrowserModel::Implementation
{
	Implementation( TestAssetBrowserModel& self );

	TestAssetBrowserModel& self_;
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
	std::vector<size_t> folderItemIndexHistory_;
	size_t currentBreadcrumbIndex_;

	void addFolderItem( const FileInfo& fileInfo );

	void generateBreadcrumbs();
	void addBreadcrumb( const char* value );
	static const size_t MAX_FOLDER_CONTENTS = 200;
	static const size_t NO_SELECTION = -1;
};

TestAssetBrowserModel::Implementation::Implementation( 
	TestAssetBrowserModel& self )
	: self_( self )
	, definitionManager_( nullptr )
	, folders_( nullptr )
	, currentSelectedAssetIndex_( -1 )
	, fileSystem_()
	, currentBreadcrumbItemIndex_( NO_SELECTION )
	, currentBreadcrumbIndex_( NO_SELECTION )
{
	generateBreadcrumbs();
}

void TestAssetBrowserModel::Implementation::generateBreadcrumbs()
{
	breadcrumbs_.clear();
}

void TestAssetBrowserModel::Implementation::addBreadcrumb( const char* value )
{
	breadcrumbs_.push_back( value );
}

void TestAssetBrowserModel::Implementation::addFolderItem(
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

TestAssetBrowserModel::TestAssetBrowserModel()
	: IAssetBrowserModel()
	, impl_( new Implementation( *this ) )
{
}

TestAssetBrowserModel::TestAssetBrowserModel( 
	const TestAssetBrowserModel& rhs )
	: IAssetBrowserModel()
	, impl_( new Implementation( *this ) )
{
	impl_->definitionManager_ = rhs.impl_->definitionManager_;
}

TestAssetBrowserModel::~TestAssetBrowserModel()
{
}

void TestAssetBrowserModel::initialise( IContextManager& contextManager )
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

void TestAssetBrowserModel::addListener( IAssetListener* listener )
{
	if (listener == nullptr)
	{
		return;
	}

	impl_->listeners_.push_back( listener );
}


const std::vector<std::string>& TestAssetBrowserModel::assetPaths() const
{
	return impl_->assetPaths_;
}

bool TestAssetBrowserModel::addAssetPath(const std::string& path)
{
	if (std::find(impl_->assetPaths_.begin(), impl_->assetPaths_.end(), path)
			!= impl_->assetPaths_.end())
	{
		return true;
	}

	if (!impl_->fileSystem_->exists( path.c_str() ))
	{
		NGT_ERROR_MSG( "TestAssetBrowserModel::populateFolderContents: "
		"assert folder path does not exist: %s\n", path.c_str() );
		return false;
	}

	impl_->assetPaths_.push_back( path );

	return true;
}


IFileSystem* TestAssetBrowserModel::fileSystem() const
{
	return impl_->fileSystem_;
}

void TestAssetBrowserModel::addFolderItems( 
	const std::vector<std::string>& paths)
{
	IFileSystem * fs = impl_->fileSystem_;

	std::list<std::string> directories;
	
	for (auto& path : paths)
	{
		if (!fs->exists( path.c_str() ))
		{
			NGT_WARNING_MSG( "TestAssetBrowserModel::populateFolderContents: "
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

void TestAssetBrowserModel::populateFolderContents( 
	const std::vector<std::string>& paths )
{
	impl_->folderContents_.clear();
	addFolderItems( paths );
}

ObjectHandle TestAssetBrowserModel::getBreadcrumbs() const
{
	return impl_->breadcrumbs_;
}

ObjectHandle TestAssetBrowserModel::getFolderContents() const
{
	return impl_->folderContents_;
}

ObjectHandle TestAssetBrowserModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
}

const char* TestAssetBrowserModel::getThumbnail( const char * filename ) const
{
	//TODO
	return "thumbnail.png";
}

const int & TestAssetBrowserModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void TestAssetBrowserModel::currentSelectedAssetIndex( const int & index )
{
	impl_->currentSelectedAssetIndex_ = index;
}

bool TestAssetBrowserModel::useSelectedAsset() const
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
bool TestAssetBrowserModel::navigateHistoryForward() const
{
	// Update the current breadcrumb item index and let the listeners know
	// the data has been changed.
	if (impl_->foldersCrumb_.size() > impl_->currentBreadcrumbIndex_ + 1)
	{
		impl_->currentBreadcrumbIndex_ += 1;
		impl_->currentBreadcrumbItemIndex_.value( impl_->currentBreadcrumbIndex_ );
	}

	return true;
}

/// Navigate backward
bool TestAssetBrowserModel::navigateHistoryBackward() const
{
	// Update the current breadcrumb item index and let the listeners know
	// the data has been changed.
	if (0 < impl_->currentBreadcrumbIndex_)
	{
		impl_->currentBreadcrumbIndex_ -= 1;
		impl_->currentBreadcrumbItemIndex_.value( impl_->currentBreadcrumbIndex_ );
	}

	return true;
}

Variant TestAssetBrowserModel::getFolderTreeItemSelected() const
{
	return Variant();
}

void TestAssetBrowserModel::setFolderTreeItemSelected( const Variant& selectedItem )
{
	auto item = reinterpret_cast< const FolderTreeItem *>( selectedItem.value<intptr_t>());
	
	if ( item )
	{
		const FileInfo& fileInfo = item->getFileInfo();
		std::vector< std::string > paths;
		paths.push_back( fileInfo.fullPath );

		std::string token = "\\";
		auto lastToken = std::find_end( fileInfo.fullPath.begin(), fileInfo.fullPath.end(), token.begin(), token.end() );

		if (impl_->folders_)
		{
			ITreeModel::ItemIndex selectedItemIndex = impl_->folders_->index( item );
			auto foundItemIndex = std::find( impl_->foldersCrumb_.begin(), impl_->foldersCrumb_.end(), selectedItemIndex );
			
			// Don't add same ItemIndex twice
			if ( impl_->foldersCrumb_.end() == foundItemIndex )
			{
				// Keep the folder item index history and update current breadcrumb index
				impl_->folderItemIndexHistory_.push_back( selectedItemIndex.first );
				impl_->currentBreadcrumbIndex_ = (impl_->folderItemIndexHistory_.size() - 1);

				impl_->foldersCrumb_.push_back( selectedItemIndex );

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

ObjectHandle TestAssetBrowserModel::currentBreadcrumbItemIndex() const
{
	return ObjectHandle( &impl_->currentBreadcrumbItemIndex_ );
}

const size_t & TestAssetBrowserModel::getCurrentBreadcrumbItemIndex() const
{
	return impl_->currentBreadcrumbIndex_;
}

void TestAssetBrowserModel::setCurrentBreadcrumbItemIndex( const size_t & index )
{
	impl_->currentBreadcrumbIndex_ = index;
	impl_->currentBreadcrumbItemIndex_.value( index );
}

size_t TestAssetBrowserModel::getFolderTreeItemIndex() const
{
	if (impl_->folderItemIndexHistory_.size() <= 0 ||
		TestAssetBrowserModel::Implementation::NO_SELECTION == impl_->currentBreadcrumbIndex_)
	{
		return 0;
	}

	return impl_->folderItemIndexHistory_[impl_->currentBreadcrumbIndex_];
}
