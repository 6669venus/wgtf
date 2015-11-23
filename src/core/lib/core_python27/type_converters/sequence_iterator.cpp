#include "pch.hpp"

#include "sequence_iterator.hpp"

#include "i_type_converter.hpp"

#include "core_variant/variant.hpp"

#include <cassert>


namespace PythonType
{


namespace Detail
{


template< typename T >
PyScript::ScriptObject getItem(
	const typename SequenceIterator< T >::container_type & container,
	const typename SequenceIterator< T >::key_type & index )
{
	return container.getItem( index );
}


template<>
PyScript::ScriptObject getItem< PyScript::ScriptSequence >(
	const typename SequenceIterator< PyScript::ScriptSequence >::container_type & container,
	const typename SequenceIterator< PyScript::ScriptSequence >::key_type & index )
{
	return container.getItem( index, PyScript::ScriptErrorPrint() );
}


template< typename T >
bool setItem(
	const typename SequenceIterator< T >::container_type & container,
	const typename SequenceIterator< T >::key_type & index,
	PyScript::ScriptObject & scriptValue )
{
	return container.setItem( index, scriptValue );
}


template<>
bool setItem< PyScript::ScriptSequence >(
	const typename SequenceIterator< PyScript::ScriptSequence >::container_type & container,
	const typename SequenceIterator< PyScript::ScriptSequence >::key_type & index,
	PyScript::ScriptObject & scriptValue )
{
	return container.setItem( index, scriptValue, PyScript::ScriptErrorPrint() );
}

} // namespace Detail


template< typename T >
SequenceIterator< T >::SequenceIterator( const container_type & container,
	typename SequenceIterator< T >::key_type index,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, index_( index )
	, typeConverters_( typeConverters )
{
	// Does not support negative indexes
	assert( index_ >= 0 );
}


template< typename T >
const typename SequenceIterator< T >::container_type &
SequenceIterator< T >::container() const
{
	return container_;
}


template< typename T >
typename SequenceIterator< T >::key_type SequenceIterator< T >::index() const
{
	return index_;
}


template< typename T >
Variant SequenceIterator< T >::key() const /* override */
{
	return Variant( index_ );
}


template< typename T >
Variant SequenceIterator< T >::value() const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: sequence index out of range\n" );
		return Variant();
	}

	PyScript::ScriptObject item = Detail::getItem< T >( container_, index_ );
	
	Variant result;
	const bool success = typeConverters_.toVariant( item, result );
	return result;
}


template< typename T >
bool SequenceIterator< T >::setValue( const Variant & value ) const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "IndexError: sequence assignment index out of range\n" );
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
void SequenceIterator< T >::inc() /* override */
{
	++index_;
}


template< typename T >
bool SequenceIterator< T >::equals(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	assert( t );
	if (!t)
	{
		return false;
	}

	assert( container_ == t->container_ );
	return (index_ == t->index_);
}


template< typename T >
bool SequenceIterator< T >::lessthan(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	assert( t );
	if (!t)
	{
		return false;
	}

	assert( container_ == t->container_ );
	return (index_ == t->index_);
}


template< typename T >
CollectionIteratorImplPtr SequenceIterator< T >::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


// Explicit instantiations
template class SequenceIterator< PyScript::ScriptList >;
template class SequenceIterator< PyScript::ScriptSequence >;
template class SequenceIterator< PyScript::ScriptTuple >;


} // namespace PythonType
