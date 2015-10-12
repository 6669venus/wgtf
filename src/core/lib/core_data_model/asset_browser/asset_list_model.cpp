#include "asset_list_model.hpp"
#include "asset_object_item.hpp"

#include "core_data_model/i_item_role.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include <thread>

AssetListModel::AssetListModel()
{
}

AssetListModel::AssetListModel( const AssetListModel& rhs )
{
}

AssetListModel::~AssetListModel()
{
}

AssetListModel& AssetListModel::operator=( const AssetListModel& rhs )
{
	return *this;
}

IItem* AssetListModel::item( size_t index ) const
{
	if (index >= items_.size())
	{
		return nullptr;
	}

	return items_.at( index ).get();
}

size_t AssetListModel::index( const IItem* item ) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert( it != items_.end() );
	return index;
}

bool AssetListModel::empty() const
{
	return items_.empty();
}

size_t AssetListModel::size() const
{
	return items_.size();
}

bool AssetListModel::canClear() const
{
	return true;
}

void AssetListModel::clear()
{
	this->resize( 0 );
}

void AssetListModel::resize( size_t newSize )
{
	auto oldSize = size();
	if (newSize < oldSize)
	{
		notifyPreItemsRemoved( nullptr, newSize, oldSize - newSize );
		items_.resize( newSize );
		notifyPostItemsRemoved( nullptr, newSize, oldSize - newSize );
	}
	else if (newSize > oldSize)
	{
		notifyPreItemsInserted( nullptr, oldSize, newSize - oldSize );
		items_.resize( newSize );
		notifyPostItemsInserted( nullptr, oldSize, newSize - oldSize );
	}
}

AssetListModel::Iterator AssetListModel::insert( const AssetListModel::Iterator & position, AssetObjectItem * value )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsInserted( nullptr, index, 1 );
	auto it = items_.emplace( position.iterator(), std::move( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );

	return it;
}

AssetListModel::Iterator AssetListModel::erase( const AssetListModel::Iterator & position )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsRemoved( nullptr, index, 1 );
	auto it = items_.erase( position.iterator() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return it;
}

AssetListModel::Iterator AssetListModel::erase(
	const AssetListModel::Iterator & first, const AssetListModel::Iterator & last )
{
	auto index = std::distance( items_.cbegin(), first.iterator() );
	auto count = std::distance( first.iterator(), last.iterator() );

	notifyPreItemsRemoved( nullptr, index, count );
	auto it = items_.erase( first.iterator(), last.iterator() );
	notifyPostItemsRemoved( nullptr, index, count );

	return it;
}

void AssetListModel::emplace_back( AssetObjectItem && value )
{
	const auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), new AssetObjectItem( std::move( value ) ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}

void AssetListModel::push_back( AssetObjectItem * value )
{
	auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), std::move( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}

void AssetListModel::push_front( AssetObjectItem * value )
{
	auto index = 0;

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.begin(), std::move( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}

AssetObjectItem AssetListModel::pop_back()
{
	auto item = items_.back().get();
	auto retVal = std::move( *item );

	auto index = items_.size() - 1;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.pop_back();
	notifyPostItemsRemoved( nullptr, index, 1 );

	return retVal;
}

AssetObjectItem AssetListModel::pop_front()
{
	auto item = items_.front().get();
	auto retVal = std::move( *item );

	auto index = 0;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.erase( items_.begin() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return retVal;
}

const AssetObjectItem & AssetListModel::back() const
{
	return *items_.back().get();
}

const AssetObjectItem & AssetListModel::front() const
{
	return *items_.front().get();
}

AssetObjectItem & AssetListModel::operator[](size_t index)
{
	return *items_[index].get();
}

const AssetObjectItem & AssetListModel::operator[](size_t index) const
{
	return *items_[index].get();
}
