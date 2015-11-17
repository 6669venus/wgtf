#include "pch.hpp"

#include "list_iterator.hpp"

#include "i_type_converter.hpp"

#include "core_variant/variant.hpp"

#include <cassert>


namespace PythonType
{


namespace Detail
{


template< typename T >
PyScript::ScriptObject getItem(
	const typename ListIteratorImpl< T >::container_type & container,
	const typename ListIteratorImpl< T >::key_type & index )
{
	return container.getItem( index );
}


template<>
PyScript::ScriptObject getItem< PyScript::ScriptSequence >(
	const typename ListIteratorImpl< PyScript::ScriptSequence >::container_type & container,
	const typename ListIteratorImpl< PyScript::ScriptSequence >::key_type & index )
{
	return container.getItem( index, PyScript::ScriptErrorPrint() );
}


template< typename T >
bool setItem(
	const typename ListIteratorImpl< T >::container_type & container,
	const typename ListIteratorImpl< T >::key_type & index,
	PyScript::ScriptObject & scriptValue )
{
	return container.setItem( index, scriptValue );
}


template<>
bool setItem< PyScript::ScriptSequence >(
	const typename ListIteratorImpl< PyScript::ScriptSequence >::container_type & container,
	const typename ListIteratorImpl< PyScript::ScriptSequence >::key_type & index,
	PyScript::ScriptObject & scriptValue )
{
	return container.setItem( index, scriptValue, PyScript::ScriptErrorPrint() );
}

} // namespace Detail


template< typename T >
ListIteratorImpl< T >::ListIteratorImpl( const container_type & container,
	typename ListIteratorImpl< T >::key_type index,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, index_( index )
	, typeConverters_( typeConverters )
{
	// ListIteratorImpl does not support negative indexes
	assert( index_ >= 0 );
}


template< typename T >
const typename ListIteratorImpl< T >::container_type &
ListIteratorImpl< T >::container() const
{
	return container_;
}


template< typename T >
typename ListIteratorImpl< T >::key_type ListIteratorImpl< T >::index() const
{
	return index_;
}


template< typename T >
Variant ListIteratorImpl< T >::key() const /* override */
{
	return Variant( index_ );
}


template< typename T >
Variant ListIteratorImpl< T >::value() const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: list index out of range\n" );
		return Variant();
	}

	PyScript::ScriptObject item = Detail::getItem< T >( container_, index_ );
	
	Variant result;
	const bool success = typeConverters_.toVariant( item, result );
	return result;
}


template< typename T >
bool ListIteratorImpl< T >::setValue( const Variant & value ) const /* override */
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

	return Detail::setItem< T >( container_, index_, scriptValue );
}


template< typename T >
void ListIteratorImpl< T >::inc() /* override */
{
	++index_;
}


template< typename T >
bool ListIteratorImpl< T >::equals(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	if (!t)
	{
		return false;
	}

	return (container_ == t->container_) &&
		(index_ == t->index_);
}


template< typename T >
CollectionIteratorImplPtr ListIteratorImpl< T >::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


// Explicit instantiations
template class ListIteratorImpl< PyScript::ScriptList >;
template class ListIteratorImpl< PyScript::ScriptSequence >;
template class ListIteratorImpl< PyScript::ScriptTuple >;


} // namespace PythonType
