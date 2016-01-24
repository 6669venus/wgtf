#include "property_storage.hpp"

#include "interfaces/i_base_property.hpp"

PropertyStorage::PropertyStorage()
{

}


PropertyStorage::~PropertyStorage()
{

}


void PropertyStorage::addProperty( const IBasePropertyPtr & property )
{
	properties_.emplace_back( property );
}


PropertyStorageIterator::PropertyStorageIterator( const PropertyStorage & storage )
	: current_( nullptr )
	, iterator_( storage.properties_.cbegin() )
	, end_( storage.properties_.cend() )
{

}


IBasePropertyPtr PropertyStorageIterator::current() const
{
	return current_;
}


bool PropertyStorageIterator::next()
{
	if (iterator_ == end_)
	{
		return false;
	}

	current_ = *iterator_;
	++iterator_;
	return true;
}
