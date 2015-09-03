#include "variant_list.hpp"
#include "i_item.hpp"
#include "i_item_role.hpp"
#include "core_variant/variant.hpp"

#ifdef __APPLE__
template<>
const Variant & VariantListItem::value<const Variant &>() const
{
	return value_;
}
#endif // __APPLE__

VariantListItem::VariantListItem( const Variant& value )
	: value_( value )
{
}


VariantListItem::VariantListItem( Variant&& value )
	: value_( std::move( value ) )
{
}


int VariantListItem::columnCount() const
{
	return 1;
}


const char * VariantListItem::getDisplayText( int column ) const
{
	return nullptr;
}


ThumbnailData VariantListItem::getThumbnail( int column ) const
{
	return nullptr;
}


Variant VariantListItem::getData( int column, size_t roleId ) const
{
	if (roleId == ValueTypeRole::roleId_)
	{
		auto type = value_.type();
		if (type != nullptr)
		{
			return type->typeId().getName();
		}
	}
	else if (roleId == ValueRole::roleId_)
	{
		return value_;
	}

	return Variant();
}


bool VariantListItem::setData( int column, size_t roleId, const Variant & data )
{
	if (roleId != ValueRole::roleId_)
	{
		return false;
	}

	value_ = data;
	return true;
}


VariantListItem & VariantListItem::operator=( const Variant & data )
{
	value_ = data;
	return *this;
}


VariantListItem & VariantListItem::operator=( Variant && data )
{
	value_ = std::move( data );
	return *this;
}


bool VariantListItem::operator==( const Variant & data ) const
{
	return value_ == data;
}


VariantList::ConstIterator::ConstIterator( const ConstIterator& rhs )
	: iterator_( new Items::const_iterator( *rhs.iterator_ ) )
{
}


VariantList::ConstIterator&
	VariantList::ConstIterator::operator=( const ConstIterator& rhs )
{
	if (this != &rhs)
	{
		iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
	}

	return *this;
}


VariantList::ConstIterator::reference VariantList::ConstIterator::operator*() const
{
	auto item = static_cast< VariantListItem * >( (*iterator_)->get() );
	return *item;
}


VariantList::ConstIterator & VariantList::ConstIterator::operator++()
{
	++(*iterator_);
	return *this;
}


VariantList::ConstIterator VariantList::ConstIterator::operator++( int )
{
	ConstIterator tmp = *this;
	++(*this);
	return tmp;
}


bool VariantList::ConstIterator::operator==( const ConstIterator & other ) const
{
	return *iterator_ == *other.iterator_;
}


bool VariantList::ConstIterator::operator!=( const ConstIterator & other ) const
{
	return !(*this == other);
}


bool VariantList::ConstIterator::operator<( const ConstIterator & other ) const
{
	return *iterator_ < *other.iterator_;
}


VariantList::ConstIterator::difference_type VariantList::ConstIterator::operator-(
	const VariantList::ConstIterator & other ) const
{
	return *iterator_ - *other.iterator_;
}

VariantList::ConstIterator VariantList::ConstIterator::operator+(
	VariantList::ConstIterator::difference_type n) const
{
	return *iterator_ + n;
}

VariantList::ConstIterator::ConstIterator( const Items::const_iterator & iterator )
	: iterator_( new Items::const_iterator( iterator ) )
{
}


const VariantList::Items::const_iterator&
VariantList::ConstIterator::iterator() const
{
	return *iterator_;
}


VariantList::Iterator::Iterator( const Iterator& rhs )
	: ConstIterator()
{
	iterator_.reset( new Items::const_iterator( rhs.iterator() ) );
}


VariantList::Iterator& VariantList::Iterator::operator=( const Iterator& rhs )
{
	if (this != &rhs)
	{
		iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
	}

	return *this;
}


VariantList::Iterator::reference VariantList::Iterator::operator*() const
{
	auto item = static_cast< VariantListItem * >( (*iterator_)->get() );
	return *item;
}


VariantList::Iterator & VariantList::Iterator::operator++()
{
	++(*iterator_);
	return *this;
}


VariantList::Iterator VariantList::Iterator::operator++( int )
{
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

VariantList::Iterator VariantList::Iterator::operator+(
	VariantList::Iterator::difference_type n) const
{
	ConstIterator itr = *iterator_ + n;
	return *(Iterator*)&itr;
}


VariantList::Iterator::Iterator( const Items::iterator & iterator )
	: ConstIterator()
{
	iterator_.reset( new Items::const_iterator( iterator ) );
}


const VariantList::Items::const_iterator& VariantList::Iterator::iterator() const
{
	return *( iterator_.get() );
}


VariantList::VariantList()
{
}


VariantList::~VariantList()
{
}


IItem * VariantList::item( size_t index ) const
{
	if (index >= items_.size())
	{
		return nullptr;
	}

	return items_[index].get();
}


size_t VariantList::index( const IItem * item ) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert( it != items_.end() );
	return index;
}


bool VariantList::empty() const
{
	return items_.empty();
}


size_t VariantList::size() const
{
	return items_.size();
}


bool VariantList::canClear() const
{
	return true;
}


void VariantList::clear()
{
	this->resize( 0 );
}


void VariantList::resize( size_t newSize )
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


VariantList::ConstIterator VariantList::cbegin() const
{
	return ConstIterator( items_.cbegin() );
}


VariantList::ConstIterator VariantList::cend() const
{
	return ConstIterator( items_.cend() );
}

VariantList::ConstIterator VariantList::begin() const
{
	return ConstIterator(items_.cbegin());
}

VariantList::ConstIterator VariantList::end() const
{
	return ConstIterator(items_.cend());
}


VariantList::Iterator VariantList::begin()
{
	return Iterator( items_.begin() );
}


VariantList::Iterator VariantList::end()
{
	return Iterator( items_.end() );
}


VariantList::Iterator VariantList::insert(
	const VariantList::Iterator & position, const Variant & value )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsInserted( nullptr, index, 1 );
	auto it = items_.emplace(
		position.iterator(), new VariantListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );

	return it;
}


VariantList::Iterator VariantList::erase(
	const VariantList::Iterator & position )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsRemoved( nullptr, index, 1 );
	auto it = items_.erase( position.iterator() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return it;
}


VariantList::Iterator VariantList::erase(
	const VariantList::Iterator & first, const VariantList::Iterator & last )
{
	auto index = std::distance( items_.cbegin(), first.iterator() );
	auto count = std::distance( first.iterator(), last.iterator() );

	notifyPreItemsRemoved( nullptr, index, count );
	auto it = items_.erase( first.iterator(), last.iterator() );
	notifyPostItemsRemoved( nullptr, index, count );

	return it;
}


void VariantList::emplace_back( Variant && value )
{
	const auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), new VariantListItem( std::move( value ) ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void VariantList::push_back( const Variant & value )
{
	auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), new VariantListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void VariantList::push_front( const Variant & value )
{
	auto index = 0;

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.begin(), new VariantListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void VariantList::emplace_back( VariantListItem * item )
{
	const auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void VariantList::push_back( VariantListItem * item )
{
	auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void VariantList::push_front( VariantListItem * item )
{
	auto index = 0;

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.begin(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


Variant VariantList::pop_back()
{
	auto item = static_cast< const VariantListItem * >( items_.back().get() );
	Variant value = item->value_;

	auto index = items_.size() - 1;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.pop_back();
	notifyPostItemsRemoved( nullptr, index, 1 );

	return value;
}


Variant VariantList::pop_front()
{
	auto item = static_cast< const VariantListItem * >( items_.front().get() );
	Variant value = item->value_;

	auto index = 0;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.erase( items_.begin() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return value;
}


const Variant & VariantList::back() const
{
	auto item = static_cast< const VariantListItem * >( items_.back().get() );
	const Variant & value = item->value_;

	return value;
}


const Variant & VariantList::front() const
{
	auto item = static_cast< const VariantListItem * >( items_.front().get() );
	const Variant & value = item->value_;

	return value;
}


VariantListItem & VariantList::operator[](size_t index)
{
	auto item = static_cast< VariantListItem * >( items_[index].get() );
	return *item;
}


const VariantListItem & VariantList::operator[](size_t index) const
{
	auto item = static_cast< const VariantListItem * >( items_[index].get() );
	return *item;
}
