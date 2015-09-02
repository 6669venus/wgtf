///////////////////////////////////////////////////////////////////////////////
// deprecated class implementation
#pragma warning (push)
#pragma warning (disable : 4996 )
#include "generic_list.hpp"
#ifdef __APPLE__
template<>
const Variant & GenericListItem::value<const Variant &>() const
{
	return value_;
}
#endif // __APPLE__

GenericListItem::GenericListItem( const Variant& value )
	: value_( value )
{
}


GenericListItem::GenericListItem( Variant&& value )
	: value_( std::move( value ) )
{
}


int GenericListItem::columnCount() const
{
	return 1;
}


const char * GenericListItem::getDisplayText( int column ) const
{
	Variant variant = getData( column, ValueRole::roleId_ );

	if (variant.typeIs< const char * >() ||
		variant.typeIs< std::string >())
	{
		std::string value;
		if (variant.tryCast( value ))
		{
			return value.c_str();
		}
	}

	return "";
}


ThumbnailData GenericListItem::getThumbnail( int column ) const
{
	return nullptr;
}


Variant GenericListItem::getData( int column, size_t roleId ) const
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


bool GenericListItem::setData( int column, size_t roleId, const Variant & data )
{
	if (roleId != ValueRole::roleId_)
	{
		return false;
	}

	value_ = data;
	return true;
}


GenericListItem & GenericListItem::operator=( const Variant & data )
{
	value_ = data;
	return *this;
}


GenericListItem & GenericListItem::operator=( Variant && data )
{
	value_ = std::move( data );
	return *this;
}


bool GenericListItem::operator==( const Variant & data ) const
{
	return value_ == data;
}


GenericList::ConstIterator::ConstIterator( const ConstIterator& rhs )
	: iterator_( new Items::const_iterator( *rhs.iterator_ ) )
{
}


GenericList::ConstIterator&
	GenericList::ConstIterator::operator=( const ConstIterator& rhs )
{
	if (this != &rhs)
	{
		iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
	}

	return *this;
}


GenericList::ConstIterator::reference GenericList::ConstIterator::operator*() const
{
	auto item = static_cast< GenericListItem * >( (*iterator_)->get() );
	return *item;
}


GenericList::ConstIterator & GenericList::ConstIterator::operator++()
{
	++(*iterator_);
	return *this;
}


GenericList::ConstIterator GenericList::ConstIterator::operator++( int )
{
	ConstIterator tmp = *this;
	++(*this);
	return tmp;
}


bool GenericList::ConstIterator::operator==( const ConstIterator & other ) const
{
	return *iterator_ == *other.iterator_;
}


bool GenericList::ConstIterator::operator!=( const ConstIterator & other ) const
{
	return !(*this == other);
}


bool GenericList::ConstIterator::operator<( const ConstIterator & other ) const
{
	return *iterator_ < *other.iterator_;
}


GenericList::ConstIterator::difference_type GenericList::ConstIterator::operator-(
	const GenericList::ConstIterator & other ) const
{
	return *iterator_ - *other.iterator_;
}

GenericList::ConstIterator GenericList::ConstIterator::operator+(
	GenericList::ConstIterator::difference_type n) const
{
	return *iterator_ + n;
}

GenericList::ConstIterator::ConstIterator( const Items::const_iterator & iterator )
	: iterator_( new Items::const_iterator( iterator ) )
{
}


const GenericList::Items::const_iterator&
GenericList::ConstIterator::iterator() const
{
	return *iterator_;
}


GenericList::Iterator::Iterator( const Iterator& rhs )
	: ConstIterator()
{
	iterator_.reset( new Items::const_iterator( rhs.iterator() ) );
}


GenericList::Iterator& GenericList::Iterator::operator=( const Iterator& rhs )
{
	if (this != &rhs)
	{
		iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
	}

	return *this;
}


GenericList::Iterator::reference GenericList::Iterator::operator*() const
{
	auto item = static_cast< GenericListItem * >( (*iterator_)->get() );
	return *item;
}


GenericList::Iterator & GenericList::Iterator::operator++()
{
	++(*iterator_);
	return *this;
}


GenericList::Iterator GenericList::Iterator::operator++( int )
{
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

GenericList::Iterator GenericList::Iterator::operator+(
	GenericList::Iterator::difference_type n) const
{
	ConstIterator itr = *iterator_ + n;
	return *(Iterator*)&itr;
}


GenericList::Iterator::Iterator( const Items::iterator & iterator )
	: ConstIterator()
{
	iterator_.reset( new Items::const_iterator( iterator ) );
}


const GenericList::Items::const_iterator& GenericList::Iterator::iterator() const
{
	return *( iterator_.get() );
}


GenericList::GenericList()
{
}


GenericList::~GenericList()
{
}


IItem * GenericList::item( size_t index ) const
{
	if (index >= items_.size())
	{
		return nullptr;
	}

	return items_[index].get();
}


size_t GenericList::index( const IItem * item ) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert( it != items_.end() );
	return index;
}


bool GenericList::empty() const
{
	return items_.empty();
}


size_t GenericList::size() const
{
	return items_.size();
}


bool GenericList::canClear() const
{
	return true;
}


void GenericList::clear()
{
	this->resize( 0 );
}


void GenericList::resize( size_t newSize )
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


GenericList::ConstIterator GenericList::cbegin() const
{
	return ConstIterator( items_.cbegin() );
}


GenericList::ConstIterator GenericList::cend() const
{
	return ConstIterator( items_.cend() );
}

GenericList::ConstIterator GenericList::begin() const
{
	return ConstIterator(items_.cbegin());
}

GenericList::ConstIterator GenericList::end() const
{
	return ConstIterator(items_.cend());
}


GenericList::Iterator GenericList::begin()
{
	return Iterator( items_.begin() );
}


GenericList::Iterator GenericList::end()
{
	return Iterator( items_.end() );
}


GenericList::Iterator GenericList::insert(
	const GenericList::Iterator & position, const Variant & value )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsInserted( nullptr, index, 1 );
	auto it = items_.emplace(
		position.iterator(), new GenericListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );

	return it;
}


GenericList::Iterator GenericList::erase(
	const GenericList::Iterator & position )
{
	auto index = std::distance( items_.cbegin(), position.iterator() );

	notifyPreItemsRemoved( nullptr, index, 1 );
	auto it = items_.erase( position.iterator() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return it;
}


GenericList::Iterator GenericList::erase(
	const GenericList::Iterator & first, const GenericList::Iterator & last )
{
	auto index = std::distance( items_.cbegin(), first.iterator() );
	auto count = std::distance( first.iterator(), last.iterator() );

	notifyPreItemsRemoved( nullptr, index, count );
	auto it = items_.erase( first.iterator(), last.iterator() );
	notifyPostItemsRemoved( nullptr, index, count );

	return it;
}


void GenericList::emplace_back( Variant && value )
{
	const auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), new GenericListItem( std::move( value ) ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void GenericList::push_back(Variant && value)
{
	emplace_back(std::move(value));
}


void GenericList::push_back( const Variant & value )
{
	auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), new GenericListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void GenericList::push_front( const Variant & value )
{
	auto index = 0;

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.begin(), new GenericListItem( value ) );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void GenericList::emplace_back( GenericListItem * item )
{
	const auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void GenericList::push_back( GenericListItem * item )
{
	auto index = items_.size();

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.end(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


void GenericList::push_front( GenericListItem * item )
{
	auto index = 0;

	notifyPreItemsInserted( nullptr, index, 1 );
	items_.emplace( items_.begin(), item );
	notifyPostItemsInserted( nullptr, index, 1 );
}


Variant GenericList::pop_back()
{
	auto item = static_cast< const GenericListItem * >( items_.back().get() );
	Variant value = item->value_;

	auto index = items_.size() - 1;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.pop_back();
	notifyPostItemsRemoved( nullptr, index, 1 );

	return value;
}


Variant GenericList::pop_front()
{
	auto item = static_cast< const GenericListItem * >( items_.front().get() );
	Variant value = item->value_;

	auto index = 0;

	notifyPreItemsRemoved( nullptr, index, 1 );
	items_.erase( items_.begin() );
	notifyPostItemsRemoved( nullptr, index, 1 );

	return value;
}


const Variant & GenericList::back() const
{
	auto item = static_cast< const GenericListItem * >( items_.back().get() );
	const Variant & value = item->value_;

	return value;
}


const Variant & GenericList::front() const
{
	auto item = static_cast< const GenericListItem * >( items_.front().get() );
	const Variant & value = item->value_;

	return value;
}


GenericListItem & GenericList::operator[](size_t index)
{
	assert(index < items_.size());
	auto item = static_cast< GenericListItem * >( items_[index].get() );
	return *item;
}


const GenericListItem & GenericList::operator[](size_t index) const
{
	assert(index < items_.size());
	auto item = static_cast< const GenericListItem * >( items_[index].get() );
	return *item;
}

#pragma warning (pop)
///////////////////////////////////////////////////////////////////////////////
