#include "pch.hpp"

#include "list_iterator.hpp"

#include "i_type_converter.hpp"

#include "core_variant/variant.hpp"

#include <cassert>


namespace PythonType
{


ListIteratorImpl::ListIteratorImpl( const container_type & container,
	ListIteratorImpl::key_type index,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, index_( index )
	, typeConverters_( typeConverters )
{
	// ListIteratorImpl does not support negative indexes
	assert( index_ >= 0 );
}


const ListIteratorImpl::container_type & ListIteratorImpl::container() const
{
	return container_;
}


ListIteratorImpl::key_type ListIteratorImpl::index() const
{
	return index_;
}


Variant ListIteratorImpl::key() const /* override */
{
	return Variant( index_ );
}


Variant ListIteratorImpl::value() const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: list index out of range\n" );
		return Variant();
	}

	PyScript::ScriptObject item = container_.getItem( index_ );
	
	Variant result;
	const bool success = typeConverters_.toVariant( item, result );
	return result;
}


bool ListIteratorImpl::setValue( const Variant & value ) const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: list assignment index out of range\n" );
		return false;
	}

	PyScript::ScriptObject scriptValue;
	const bool success = typeConverters_.toScriptType( value, scriptValue );
	if (!success)
	{
		return false;
	}

	return container_.setItem( index_, scriptValue );
}


void ListIteratorImpl::inc() /* override */
{
	++index_;
}


bool ListIteratorImpl::equals(
	const CollectionIteratorImplBase& that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	if (!t)
	{
		return false;
	}

	return (container_ == t->container_) &&
		(index_ == t->index_);
}


CollectionIteratorImplPtr ListIteratorImpl::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


} // namespace PythonType
