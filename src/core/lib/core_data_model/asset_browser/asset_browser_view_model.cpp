//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_view_model.hpp"
#include "i_asset_browser_model.hpp"
#include "i_asset_browser_event_model.hpp"
#include "i_asset_object_model.hpp"

#include "data_model/generic_list.hpp"
#include "data_model/value_change_notifier.hpp"
#include "data_model/i_tree_model.hpp"
#include "data_model/i_item_role.hpp"

static const size_t NO_SELECTION = SIZE_MAX;

struct AssetBrowserViewModel::AssetBrowserViewModelImplementation
{
	AssetBrowserViewModelImplementation(ObjectHandle data, ObjectHandle events)
		: currentSelectedAssetIndex_( -1 )
		, currentBreadcrumbItemIndex_( NO_SELECTION )
		, data_( std::move(data) )
		, events_( std::move(events) )
	{
	}

	void addBreadcrumb( const char* value )
	{
		breadcrumbs_.push_back( value );
	}

	void generateBreadcrumbs(const IItem* selectedItem)
	{
		auto folders = data_.getBase<IAssetBrowserModel>()->getFolderTreeModel().getBase<ITreeModel>();
		if ( selectedItem && folders )
		{
			ITreeModel::ItemIndex selectedItemIndex = folders->index( selectedItem );
			auto foundItemIndex = std::find( foldersCrumb_.begin(), foldersCrumb_.end(), selectedItemIndex );

			// Don't add same ItemIndex twice
			if ( foldersCrumb_.end() == foundItemIndex )
			{
				// Keep the folder item index history and update current breadcrumb index
				folderItemIndexHistory_.push_back( selectedItemIndex.first );
				currentBreadcrumbIndex_ = ( folderItemIndexHistory_.size() - 1 );

				foldersCrumb_.push_back( selectedItemIndex );

				auto assetObjectModel = selectedItem->getData(0, ValueRole::roleId_).cast<ObjectHandle>();

				addBreadcrumb( assetObjectModel.getBase<IAssetObjectModel>()->getFileName() );
			}
		}
	}

	GenericList	breadcrumbs_;
	int			currentSelectedAssetIndex_;
	size_t		currentBreadcrumbIndex_;

	std::vector<ITreeModel::ItemIndex>	foldersCrumb_;
	ValueChangeNotifier< size_t >		currentBreadcrumbItemIndex_;
	std::vector<size_t>					folderItemIndexHistory_;

	ObjectHandle	data_;
	ObjectHandle	events_;
};

AssetBrowserViewModel::AssetBrowserViewModel(ObjectHandle data, ObjectHandle events ) :
	impl_( new AssetBrowserViewModelImplementation( std::move(data), std::move(events) ) )
{
	auto eventModel = impl_->events_.getBase<IAssetBrowserEventModel>();
	if(eventModel)
	{
		eventModel->connectNavigateHistoryForward([&](){ onNavigateHistoryForward(); });
		eventModel->connectNavigateHistoryBackward([&](bool val){ onNavigateHistoryBackward(); });
	}
}

ObjectHandle AssetBrowserViewModel::data() const
{
	return impl_->data_;
}

ObjectHandle AssetBrowserViewModel::events() const
{
	return impl_->events_;
}

ObjectHandle AssetBrowserViewModel::getBreadcrumbs() const
{
	return impl_->breadcrumbs_;
}

Variant AssetBrowserViewModel::getFolderTreeItemSelected() const
{
	return Variant();
}

void AssetBrowserViewModel::setFolderTreeItemSelected( const Variant& selectedItem )
{
	auto selectedElement = reinterpret_cast<IItem*>( selectedItem.value<intptr_t>() );

	impl_->data_.getBase<IAssetBrowserModel>()->populateFolderContents( selectedElement );

	impl_->generateBreadcrumbs( selectedElement );
}

size_t AssetBrowserViewModel::getFolderTreeItemIndex() const
{
	if ( impl_->folderItemIndexHistory_.size() <= 0 ||
		NO_SELECTION == impl_->currentBreadcrumbIndex_ )
	{
		return 0;
	}

	return impl_->folderItemIndexHistory_[impl_->currentBreadcrumbIndex_];
}

ObjectHandle AssetBrowserViewModel::currentBreadcrumbItemIndex() const
{
	return ObjectHandle( &impl_->currentBreadcrumbItemIndex_ );
}

const size_t & AssetBrowserViewModel::getCurrentBreadcrumbItemIndex() const
{
	return impl_->currentBreadcrumbIndex_;
}

void AssetBrowserViewModel::setCurrentBreadcrumbItemIndex( const size_t & index )
{
	impl_->currentBreadcrumbIndex_ = index;
	impl_->currentBreadcrumbItemIndex_.value( index );
}

const int & AssetBrowserViewModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void AssetBrowserViewModel::currentSelectedAssetIndex( const int & index )
{
	impl_->currentSelectedAssetIndex_ = index;
}

void AssetBrowserViewModel::onNavigateHistoryForward()
{
	// Update the current breadcrumb item index
	if ( impl_->foldersCrumb_.size() > impl_->currentBreadcrumbIndex_ + 1 )
	{
		impl_->currentBreadcrumbIndex_ += 1;
		impl_->currentBreadcrumbItemIndex_.value( impl_->currentBreadcrumbIndex_ );
	}
}

void AssetBrowserViewModel::onNavigateHistoryBackward()
{
	// Update the current breadcrumb item index
	if ( 0 < impl_->currentBreadcrumbIndex_ )
	{
		impl_->currentBreadcrumbIndex_ -= 1;
		impl_->currentBreadcrumbItemIndex_.value( impl_->currentBreadcrumbIndex_ );
	}
}
