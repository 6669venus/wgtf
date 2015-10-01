/*
This model uses an index map to map indices from a source model to filtered
indices exposed by this model. Both the source model and the filtered model
consist of a list of IItem*. The map is a vector of indices pointing to the
indices in the source model that are included in this model. Only indices of
items that satisfy the filter function are included.

An example of a model could be:
item   index
AAAA   0
BBBB   1
CCCC   2
DDDD   3
EEEE   4

A filtered model of that model could be:
item   index
BBBB   1
CCCC   2
EEEE   4

The mapped indices would be:
1,2,4

If a filter is changed, the source model didn't change, but our mapping is
outdated. This is updated by a refresh. Refresh needs to do the following:
For each item in the source model, check if it was in the filter previously
and also if it satisfies the new filter. An item is in the filter if it matches
the filter function. If the before and after checks are the same, then nothing
needs to be done, but if it's different the mapped index needs to either be
removed (if it's not in the filter anymore), or added (if it's in the filter
now).

If something in the source model changed, the source model indices are
different, but the filter is the same. This is handled by our event handlers
preItemsInserted, postItemsInserted, preItemsRemoved, postItemsRemoved,
preDataChanged, and postDataChanged. [1] If an item is new and matches the
filter, it needs to be added to the index map. For every item inserted, all
indices after the insert point needs to be adjusted by adding one, because
these pointed to items that moved up when the item was inserted into the source
model. [2] If an item is removed and it exists in the index map, it needs to be
removed. For every item removed, all indices after the remove point need to be
adjusted by subtracting one, because these pointed to items that moved down the
list when the item was removed from the source model. [3] For items
that changed, the new value could mean that it does/doesn't satisfy the filter
anymore. If nothing changed, the index map stays the same, but the event needs
to be passed on if the item is in the filter. If not in the filter anymore, it
needs to be removed from the index map, and if now in the filter, it needs to be
added to the index map.

Events:
For every insert, a preItemsInserted needs to be fired before the change, and
a postItemsInserted after the change.
For every delete, a preItemsRemoved needs to be fired before the change, and
a postItemsRemoved after the change.
For every change in the item, a preDataChanged needs to be fired before the
change, and a postDataChange after the change.

This is to tell the view to update its data.
*/


#include "filtered_list_model.hpp"

#include "core_data_model/i_item.hpp"

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

struct FilteredListModel::Implementation
{
	enum class FilterUpdateType
	{
		UPDATE,
		INSERT,
		REMOVE,
		IGNORE
	};

	typedef std::vector< size_t > IndexMap;

	Implementation( FilteredListModel & self );
	Implementation( FilteredListModel & self, const FilteredListModel::Implementation & rhs );
	~Implementation();

	void initialize();
	void haltRemapping();

	void setSource( IListModel * source );

	void mapIndices();
	void remapIndices();
	void copyIndices( IndexMap& target ) const;

	void removeIndex( size_t index );
	void insertIndex( size_t index, size_t sourceIndex );
	void findItemsToRemove( size_t sourceIndex, size_t sourceCount, size_t& removeFrom, size_t& removeCount );

	FilterUpdateType checkUpdateType( size_t sourceIndex , size_t& newIndex ) const;
	void updateItem( size_t sourceIndex, size_t index, FilterUpdateType type );

	void preDataChanged( const IListModel * sender, const IListModel::PreDataChangedArgs & args );
	void postDataChanged( const IListModel * sender, const IListModel::PostDataChangedArgs & args );
	void preItemsInserted( const IListModel * sender, const IListModel::PreItemsInsertedArgs & args );
	void postItemsInserted( const IListModel * sender, const IListModel::PostItemsInsertedArgs & args );
	void preItemsRemoved( const IListModel * sender, const IListModel::PreItemsRemovedArgs & args );
	void postItemsRemoved( const IListModel * sender, const IListModel::PostItemsRemovedArgs & args );

	/// Is the item found by invoking filterFound_ function
	bool filterMatched( const IItem * item ) const;

	struct UpdateData
	{
		UpdateData()
			: item_( nullptr ), index_( 0 ), count_( 0 )
		{}

		void set(
			const IItem* item = nullptr,
			size_t index = 0, size_t count = 0 )
		{
			item_	= item;
			index_	= index;
			count_	= count;
		}

		const IItem* item_;
		size_t index_;
		size_t count_;
	} lastUpdateData_;

	FilteredListModel & self_;
	IListModel * model_;
	IItemFilter * listFilter_;
	mutable std::recursive_mutex indexMapMutex_;
	mutable std::mutex refreshMutex_;
	std::atomic_uint_fast8_t remapping_;
	std::atomic<bool> stopRemapping_;
	IndexMap indexMap_;
};

FilteredListModel::Implementation::Implementation( FilteredListModel & self )
	: self_( self )
	, model_( nullptr )
	, listFilter_( nullptr )
{
	mapIndices();
	initialize();
}

FilteredListModel::Implementation::Implementation( FilteredListModel & self, const FilteredListModel::Implementation & rhs )
	: self_( self )
	, model_( rhs.model_ )
	, listFilter_( rhs.listFilter_ )
{
	rhs.copyIndices( indexMap_ );
	initialize();
}

FilteredListModel::Implementation::~Implementation()
{
}

void FilteredListModel::Implementation::initialize()
{
	remapping_ = 0;
	stopRemapping_ = false;
}

void FilteredListModel::Implementation::haltRemapping()
{
	stopRemapping_ = true;

	while (remapping_ > 0)
	{
		std::this_thread::yield();
	}

	setSource( nullptr );
}

void FilteredListModel::Implementation::setSource( IListModel * source )
{
	if (model_ != nullptr)
	{
		model_->onPreDataChanged().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preDataChanged >( this );
		model_->onPostDataChanged().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postDataChanged >( this );
		model_->onPreItemsInserted().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsInserted >( this );
		model_->onPostItemsInserted().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsInserted >( this );
		model_->onPreItemsRemoved().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsRemoved >( this );
		model_->onPostItemsRemoved().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsRemoved >( this );
	}
	model_ = source;
	if (model_ != nullptr)
	{
		model_->onPreDataChanged().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preDataChanged >( this );
		model_->onPostDataChanged().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postDataChanged >( this );
		model_->onPreItemsInserted().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsInserted >( this );
		model_->onPostItemsInserted().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsInserted >( this );
		model_->onPreItemsRemoved().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsRemoved >( this );
		model_->onPostItemsRemoved().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsRemoved >( this );
	}
}

void FilteredListModel::Implementation::mapIndices()
{
	if (model_ == nullptr)
	{
		return;
	}

	std::lock_guard<std::recursive_mutex> lock( indexMapMutex_ );
	indexMap_.clear();
	size_t itemCount = model_->size();

	for (size_t index = 0; index < itemCount; ++index)
	{
		const IItem * item = model_->item( index );

		if (filterMatched( item ))
		{
			indexMap_.push_back( index );
		}
	}
}

void FilteredListModel::Implementation::remapIndices()
{
	++remapping_;
	std::lock_guard<std::mutex> guard( refreshMutex_ );

	if (model_ == nullptr)
	{
		--remapping_;
		return;
	}

	size_t modelCount = model_->size();
	size_t index = 0;

	for (size_t i = 0; i < modelCount; ++i)
	{
		const IItem* item = model_->item( i );

		bool itemInFilter = filterMatched( item );
		bool indexInList = index < indexMap_.size() && indexMap_[index] == i;

		if (itemInFilter && indexInList)
		{
			// Do nothing and move to the next item
			++index;
		}
		else if (!itemInFilter && indexInList)
		{
			self_.notifyPreItemsRemoved( item, index, 1 );
			removeIndex( index );
			self_.notifyPostItemsRemoved( item, index, 1 );
		}
		else if (itemInFilter)
		{
			self_.notifyPreItemsInserted( item, index, 1 );
			insertIndex( index, i );
			self_.notifyPostItemsInserted( item, index, 1 );
			++index;
		}

		if (stopRemapping_)
		{
			break;
		}
	}

	if (!stopRemapping_)
	{
		indexMap_.resize( index );
	}

	--remapping_;
}

void FilteredListModel::Implementation::copyIndices( IndexMap& target ) const
{
	std::lock( refreshMutex_, indexMapMutex_ );
	target = indexMap_;
	indexMapMutex_.unlock();
	refreshMutex_.unlock();
}

void FilteredListModel::Implementation::removeIndex( size_t index )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	indexMap_.erase( indexMap_.begin() + index );
}

void FilteredListModel::Implementation::insertIndex( size_t index, size_t sourceIndex )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	auto itr = indexMap_.begin() + index;
	indexMap_.insert( itr, sourceIndex );
}

/// Find the starting index and number of indices to remove.
void FilteredListModel::Implementation::findItemsToRemove(
	size_t sourceIndex, size_t sourceCount, size_t& removeFrom, size_t& removeCount )
{
	size_t lastSourceIndex = sourceIndex + sourceCount - 1;
	//size_t max = std::min( lastSourceIndex + 1, indexMap_.size() );
	//bool foundone = false;
	removeCount = 0;

	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), sourceIndex );

	if (itr != indexMap_.end() && *itr <= lastSourceIndex)
	{
		removeFrom = itr - indexMap_.begin();

		for (; itr != indexMap_.end() && *itr <= lastSourceIndex; ++itr)
		{
			++removeCount;
		}
	}
}

FilteredListModel::Implementation::FilterUpdateType FilteredListModel::Implementation::checkUpdateType(
	size_t sourceIndex, size_t& newIndex ) const
{
	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), sourceIndex );
	const IItem* item = model_->item( sourceIndex );
	bool wasInFilter = itr != indexMap_.end();
	bool nowInFilter = filterMatched( item );
	newIndex = wasInFilter ? *itr : 0;

	if (nowInFilter && !wasInFilter)
	{
		return FilterUpdateType::INSERT;
	}

	if (!nowInFilter && wasInFilter)
	{
		return FilterUpdateType::REMOVE;
	}

	if (nowInFilter && wasInFilter)
	{
		return FilterUpdateType::UPDATE;
	}

	return FilterUpdateType::IGNORE;
}

void FilteredListModel::Implementation::updateItem( size_t sourceIndex, size_t index, FilterUpdateType type )
{
	switch (type)
	{
	case FilterUpdateType::INSERT:
		{
			insertIndex( index, sourceIndex );
			break;
		}

	case FilterUpdateType::REMOVE:
		{
			removeIndex( index );
			break;
		}
	default:
		break;
	}
}

void FilteredListModel::Implementation::preDataChanged( const IListModel* sender, const IListModel::PreDataChangedArgs& args )
{
	std::lock_guard<std::mutex> guard( refreshMutex_ );
	self_.notifyPreDataChanged(
		args.item_, args.column_, args.roleId_, args.data_ );
	indexMapMutex_.lock();
}

void FilteredListModel::Implementation::postDataChanged( const IListModel * sender, const IListModel::PostDataChangedArgs& args )
{
	indexMapMutex_.unlock();
	std::lock_guard<std::mutex> guard( refreshMutex_ );

	size_t newIndex;
	size_t sourceIndex = model_->index( args.item_ );
	FilterUpdateType updateType = checkUpdateType( sourceIndex, newIndex );

	self_.notifyPostDataChanged(
		args.item_, args.column_, args.roleId_, args.data_ );

	switch (updateType)
	{
	case FilterUpdateType::INSERT:
		self_.notifyPreItemsInserted( args.item_, newIndex, 1 );
		updateItem( sourceIndex, newIndex, updateType );
		self_.notifyPostItemsInserted( args.item_, newIndex, 1 );
		break;

	case FilterUpdateType::REMOVE:
		self_.notifyPreItemsRemoved( args.item_, newIndex, 1 );
		updateItem( sourceIndex, newIndex, updateType );
		self_.notifyPostItemsRemoved( args.item_, newIndex, 1 );
		break;
	default:
		break;
	}
}

void FilteredListModel::Implementation::preItemsInserted( const IListModel * sender, const IListModel::PreItemsInsertedArgs & args )
{
	indexMapMutex_.lock();
}

void FilteredListModel::Implementation::postItemsInserted( const IListModel * sender, const IListModel::PostItemsInsertedArgs & args )
{
	indexMapMutex_.unlock();

	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), args.index_ );
	size_t newIndex = itr - indexMap_.begin();
	size_t max = args.index_ + args.count_;
	IndexMap newIndices;

	for (size_t i = args.index_; i < max; ++i)
	{
		const IItem* item = model_->item( i );

		if (filterMatched( item ))
		{
			newIndices.push_back( i );
		}
	}

	if (0 < newIndices.size())
	{
		size_t newCount = newIndices.size();
		self_.notifyPreItemsInserted( args.item_, newIndex, newCount );
		indexMap_.resize( indexMap_.size() + newCount );

		// Shift down all prev indices from new insertion point in case we are inserting multiple items in the middle of the indexMap_
		for (size_t i = indexMap_.size() - 1; i >= newIndex + newCount; --i)
		{
			indexMap_[i] = indexMap_[i - newCount] + args.count_;
		}

		// Insert the new indices into indexMap_
		for (size_t i = newIndex; i < newIndex + newCount; ++i)
		{
			indexMap_[i] = newIndices[i - newIndex];
		}

		self_.notifyPostItemsInserted( args.item_, newIndex, newCount );
	}
}

void FilteredListModel::Implementation::preItemsRemoved( const IListModel * sender, const IListModel::PreItemsRemovedArgs & args )
{
	indexMapMutex_.lock();
	findItemsToRemove( args.index_, args.count_, lastUpdateData_.index_, lastUpdateData_.count_ );

	if (lastUpdateData_.count_)
	{
		lastUpdateData_.item_ = args.item_;
		self_.notifyPreItemsRemoved(
			lastUpdateData_.item_, lastUpdateData_.index_, lastUpdateData_.count_ );

		size_t max = lastUpdateData_.index_ + lastUpdateData_.count_;

		if (max < indexMap_.size())
		{
			max = indexMap_.size() - lastUpdateData_.count_;

			for (size_t i = lastUpdateData_.index_; i < max; ++i)
			{
				indexMap_[i] = indexMap_[i + lastUpdateData_.count_] - args.count_;
			}
		}

		indexMap_.resize( indexMap_.size() - lastUpdateData_.count_ );
	}
}

void FilteredListModel::Implementation::postItemsRemoved( const IListModel* sender, const IListModel::PostItemsRemovedArgs& args )
{
	indexMapMutex_.unlock();

	if (lastUpdateData_.count_)
	{
		self_.notifyPostItemsRemoved(
			lastUpdateData_.item_, lastUpdateData_.index_, lastUpdateData_.count_ );
		lastUpdateData_.set();
	}
}

bool FilteredListModel::Implementation::filterMatched( const IItem * item ) const
{
	bool matched = listFilter_ ? item != nullptr && listFilter_->checkFilter( item ) : true;
	return matched;
}

FilteredListModel::FilteredListModel()
	: impl_( new Implementation( *this ) )
{}

FilteredListModel::FilteredListModel( const FilteredListModel& rhs )
	: impl_( new Implementation( *this, *rhs.impl_ ) )
{}

FilteredListModel::~FilteredListModel()
{
	impl_->haltRemapping();
}

FilteredListModel & FilteredListModel::operator=( const FilteredListModel & rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, *rhs.impl_ ) );
	}

	return *this;
}

IItem * FilteredListModel::item( size_t index ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );

	if (index >= impl_->indexMap_.size())
	{
		return nullptr;
	}

	return impl_->model_->item( impl_->indexMap_[index] );
}

size_t FilteredListModel::index( const IItem* item ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	size_t returnIndex = 0;
	size_t count = impl_->indexMap_.size();

	for (size_t i = 0; i < count; ++i)
	{
		const IItem * listItem = impl_->model_->item( impl_->indexMap_[i] );

		if (item == listItem)
		{
			returnIndex = i;
			break;
		}
	}

	return returnIndex;
}

bool FilteredListModel::empty() const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	return impl_->indexMap_.empty();
}

size_t FilteredListModel::size() const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	return impl_->indexMap_.size();
}

void FilteredListModel::setSource( IListModel * source )
{
	// Kill any current remapping going on in the background
	impl_->haltRemapping();

	// Initialize and remap the indices based on the new source
	std::lock_guard<std::mutex> guard( impl_->refreshMutex_ );

	// Set the new source
	impl_->setSource( source );

	impl_->mapIndices();
	impl_->initialize();
}

void FilteredListModel::setFilter( IItemFilter * filter )
{
	impl_->listFilter_ = filter;
	refresh();
}

IListModel* FilteredListModel::getSource()
{
	return impl_->model_;
}

const IListModel* FilteredListModel::getSource() const
{
	return impl_->model_;
}

void FilteredListModel::refresh( bool wait )
{
	if (impl_->model_ == nullptr)
	{
		return;
	}

	// evgenys: filtering in a paralel thread not supported yet.
	wait = true;

	if (wait)
	{
		impl_->remapIndices();
	}
	else
	{
		void (FilteredListModel::Implementation::*refreshMethod)() = &FilteredListModel::Implementation::remapIndices;

		std::thread nextRefresh( std::bind( refreshMethod, impl_.get() ) );

		// Let the thread to execute independently
		nextRefresh.detach();
	}
}
