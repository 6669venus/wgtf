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

#include "data_model/i_item.hpp"

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

struct FilteredListModel::Implementation
{
	typedef std::vector< size_t > ItemIndicesList;

	Implementation( FilteredListModel & filteredListModel, IListModel & model, const ListFilter & listFilterFunc );

	Implementation( FilteredListModel & filteredListModel, const FilteredListModel::Implementation & rhs );

	~Implementation();

	void initialize();
	void repopulateMappedIndices();

	bool empty() const;

	void refresh();
	void removeIndex( size_t offset );
	void insertIndex( size_t index );
	bool isInIndicesList( size_t index );
	void removeIndices( const ItemIndicesList & itemIndicesList );
	void insertIndices( const ItemIndicesList & itemIndicesList );
	void findItemsToRemove( size_t index, size_t count, ItemIndicesList & itemIndicesToRemove );

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

	FilteredListModel & filteredListModel_;
	IListModel & listModel_;
	ListFilter listFilterFunc_;
	mutable std::mutex itemListMutex_;
	ItemIndicesList itemIndicesList_;
};

FilteredListModel::Implementation::Implementation( FilteredListModel & filteredListModel, IListModel & listModel, const ListFilter & listFilterFunc )
	: filteredListModel_( filteredListModel )
	, listModel_( listModel )
	, listFilterFunc_( listFilterFunc )
{
	initialize();
}

FilteredListModel::Implementation::Implementation( FilteredListModel & filteredListModel, const FilteredListModel::Implementation & rhs )
	: filteredListModel_( filteredListModel )
	, listModel_( rhs.listModel_ )
	, listFilterFunc_( rhs.listFilterFunc_ )
{
	initialize();
}

FilteredListModel::Implementation::~Implementation()
{
	listModel_.onPreDataChanged().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preDataChanged >( this );
	listModel_.onPostDataChanged().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postDataChanged >( this );
	listModel_.onPreItemsInserted().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsInserted >( this );
	listModel_.onPostItemsInserted().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsInserted >( this );
	listModel_.onPreItemsRemoved().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsRemoved >( this );
	listModel_.onPostItemsRemoved().remove< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsRemoved >( this );
}

void FilteredListModel::Implementation::initialize()
{
	listModel_.onPreDataChanged().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preDataChanged >( this );
	listModel_.onPostDataChanged().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postDataChanged >( this );
	listModel_.onPreItemsInserted().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsInserted >( this );
	listModel_.onPostItemsInserted().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsInserted >( this );
	listModel_.onPreItemsRemoved().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::preItemsRemoved >( this );
	listModel_.onPostItemsRemoved().add< FilteredListModel::Implementation, &FilteredListModel::Implementation::postItemsRemoved >( this );

	// Populate the indices list with our list model item indices
	repopulateMappedIndices();
}

/// Clear the indices list and repopulate the list
void FilteredListModel::Implementation::repopulateMappedIndices()
{
	itemIndicesList_.clear();
	size_t itemCount = listModel_.size();
	for (size_t index = 0; index < itemCount; ++index)
	{
		const IItem * item = listModel_.item( index );
		if (filterMatched( item ))
		{
			itemIndicesList_.push_back( index );
		}
	}
}

bool FilteredListModel::Implementation::empty() const
{
	const size_t max = listModel_.size();
	for (size_t index = 0; index < max; ++index)
	{
		const IItem* listItem = listModel_.item( index );

		if (this->filterMatched( listItem ))
		{
			return false;
		}
	}

	return true;
}

void FilteredListModel::Implementation::refresh()
{
	size_t modelCount = listModel_.size();
	size_t index = 0;

	for (size_t i = 0; i < modelCount; ++i)
	{
		const IItem* item = listModel_.item( i );

		bool itemInFilter = filterMatched( item );
		bool indexInList = isInIndicesList( i );

		if (itemInFilter && indexInList)
		{
			// Do nothing and move to the next item
			++index;
		}
		else if (!itemInFilter && indexInList)
		{
			// Notify the model to remove it
			filteredListModel_.notifyPreItemsRemoved( item, index, 1 );

			removeIndex( index );

			filteredListModel_.notifyPostItemsRemoved( item, index, 1 );
		}
		else if (itemInFilter)
		{
			// Notify the model to insert it
			filteredListModel_.notifyPreItemsInserted( item, index, 1 );

			// Insert the list model index, not the filtered model index.
			insertIndex( i );

			filteredListModel_.notifyPostItemsInserted( item, index, 1 );

			++index;
		}
	}

	itemIndicesList_.resize( index );
}

/// Remove an index off the item indices list by using an offset
void FilteredListModel::Implementation::removeIndex( size_t offset )
{
	if (itemIndicesList_.size() > offset)
	{
		itemIndicesList_.erase( itemIndicesList_.begin() + offset );
	}
}

/// Insert an index into the item indices list
void FilteredListModel::Implementation::insertIndex( size_t index )
{
	// Find the correct place to insert first
	auto itr = std::lower_bound( itemIndicesList_.begin(), itemIndicesList_.end(), index );
	itemIndicesList_.insert( itr, index );
}

/// Returns true if we already have this index in our item indices list
bool FilteredListModel::Implementation::isInIndicesList( size_t index )
{
	/// Is this index in our indices list?
	ItemIndicesList::iterator itemIndicesIter = std::find( itemIndicesList_.begin(), itemIndicesList_.end(), index );
	return (itemIndicesList_.end() != itemIndicesIter);
}

void FilteredListModel::Implementation::removeIndices( const ItemIndicesList & itemIndicesList  )
{
	for (auto & iter : itemIndicesList)
	{
		auto itemAtIndex = std::find( itemIndicesList_.begin(), itemIndicesList_.end(), iter );
		if (itemIndicesList_.end() != itemAtIndex)
		{
			// Remove this item
			itemIndicesList_.erase( itemAtIndex, itemIndicesList_.end() );
		}
	}
}

void FilteredListModel::Implementation::insertIndices( const ItemIndicesList & itemIndicesList )
{
	for (auto & iter : itemIndicesList)
	{
		insertIndex( iter );
	}
}

/// Figure out what indices we want to remove off the mapped indices list.
/// First, check the model to see if the item matches the filter. Second, see if we have the item index
/// in our mapped indices list.
/// NOTE: the itemIndicesToRemove will contain the list of indices to be removed.
void FilteredListModel::Implementation::findItemsToRemove( size_t index, size_t count, ItemIndicesList & itemIndicesToRemove )
{
	for ( size_t i = index; (i < index + count && i < listModel_.size() ); ++i)
	{
		const IItem * item = listModel_.item( i );

		// See if this item matches the filter
		if (filterMatched( item ))
		{
			// See if we have this index in our mapped indices list
			auto indicesIter = std::find( itemIndicesList_.begin(), itemIndicesList_.end(), i );
			if (itemIndicesList_.end() != indicesIter)
			{
				// Build a list of removal indices off the mapped indices list
				itemIndicesToRemove.push_back( i );
			}
		}
	}
}

void FilteredListModel::Implementation::preDataChanged( const IListModel* sender, const IListModel::PreDataChangedArgs& args )
{
	filteredListModel_.notifyPreDataChanged(
		args.item_, args.column_, args.roleId_, args.data_ );
	itemListMutex_.lock();
}

void FilteredListModel::Implementation::postDataChanged( const IListModel * sender, const IListModel::PostDataChangedArgs& args )
{
	itemListMutex_.unlock();

	filteredListModel_.notifyPostDataChanged(
		args.item_, args.column_, args.roleId_, args.data_ );
}

void FilteredListModel::Implementation::preItemsInserted( const IListModel * sender, const IListModel::PreItemsInsertedArgs & args )
{
	itemListMutex_.lock();
}

void FilteredListModel::Implementation::postItemsInserted( const IListModel * sender, const IListModel::PostItemsInsertedArgs & args )
{
	itemListMutex_.unlock();

	bool indexInList = isInIndicesList( args.index_ );

	if (!indexInList)
	{
		auto itr = std::lower_bound( itemIndicesList_.begin(), itemIndicesList_.end(), args.index_ );
		size_t newIndex = itr - itemIndicesList_.begin();
		size_t max = args.index_ + args.count_;

		ItemIndicesList newIndices;
		for (size_t i = args.index_; i < max; ++i)
		{
			const IItem* item = listModel_.item( i );

			if (filterMatched( item ))
			{
				newIndices.push_back( i );
			}
		}

		filteredListModel_.notifyPreItemsInserted( args.item_, newIndex, newIndices.size() );

		if (0 < itemIndicesList_.size())
		{
			// Update the indices list from where new items will be inserted to
			// the end of the indices list.
			auto updateIter = std::lower_bound( itemIndicesList_.begin(),
												itemIndicesList_.end(),
												args.index_ );
			for (; itemIndicesList_.end() != updateIter; ++updateIter)
			{
				*updateIter += args.count_;
			}
		}

		insertIndices( newIndices );

		filteredListModel_.notifyPostItemsInserted( args.item_, newIndex, newIndices.size() );
	}
}

void FilteredListModel::Implementation::preItemsRemoved( const IListModel * sender, const IListModel::PreItemsRemovedArgs & args )
{
	lastUpdateData_.set();

	bool indexInList = isInIndicesList( args.index_ );

	itemListMutex_.lock();

	if (indexInList)
	{
		filteredListModel_.notifyPreItemsRemoved( args.item_, args.index_, args.count_ );

		// Figure out what indices we want to remove
		ItemIndicesList itemIndicesToRemove;
		findItemsToRemove( args.index_, args.count_, itemIndicesToRemove );

		// Figure out the start update index
		size_t startUpdateIndex = 0;
		auto updateIter = std::find( itemIndicesList_.begin(),
									 itemIndicesList_.end(),
									 args.index_ );

		if (itemIndicesList_.end() != updateIter)
		{
			// Cache the index to update after indices removal
			startUpdateIndex = *updateIter;
		}

		removeIndices( itemIndicesToRemove );

		if (0 < itemIndicesList_.size())
		{
			// Update the list starting from the index we cached before the removal.
			updateIter = std::find( itemIndicesList_.begin(),
									itemIndicesList_.end(),
									startUpdateIndex );
			for (; itemIndicesList_.end() != updateIter; ++updateIter)
			{
				*updateIter -= args.count_;
			}
		}

		lastUpdateData_.set( args.item_, args.index_, args.count_ );
	}
}

void FilteredListModel::Implementation::postItemsRemoved( const IListModel* sender, const IListModel::PostItemsRemovedArgs& args )
{
	itemListMutex_.unlock();

	if (lastUpdateData_.item_ != nullptr)
	{
		filteredListModel_.notifyPostItemsRemoved( lastUpdateData_.item_, lastUpdateData_.index_, lastUpdateData_.count_ );

		lastUpdateData_.set();
	}
}

bool FilteredListModel::Implementation::filterMatched( const IItem * item ) const
{
	bool matched = item != nullptr && listFilterFunc_( item );
	return matched;
}

FilteredListModel::FilteredListModel( IListModel & listModel, const ListFilter & function )
	: impl_( new Implementation( *this, listModel, function ) )
{}

FilteredListModel::FilteredListModel( const FilteredListModel& rhs )
	: impl_( new Implementation( *this, *rhs.impl_ ) )
{}

FilteredListModel::~FilteredListModel()
{}

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
	IItem * item = nullptr;
	if (impl_->listModel_.size() != impl_->itemIndicesList_.size())
	{
		// The list model has a filter, must use the converted index to 
		// grab the correct item.
		item = impl_->listModel_.item( impl_->itemIndicesList_[index] );
	}
	else
	{
		// No filter present, use the index passed to us.
		item = impl_->listModel_.item( index );
	}

	return item;
}

size_t FilteredListModel::index( const IItem* item ) const
{
	size_t returnIndex = 0;
	size_t count = impl_->listModel_.size();
	for (size_t i = 0; i < count; ++i)
	{
		const IItem * listItem = impl_->listModel_.item( i );

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
	return impl_->empty();
}

size_t FilteredListModel::size() const
{
	return impl_->listModel_.size();
}

IListModel* FilteredListModel::getSource()
{
	return &impl_->listModel_;
}

const IListModel* FilteredListModel::getSource() const
{
	return &impl_->listModel_;
}

void FilteredListModel::refresh()
{
	impl_->refresh();
}

