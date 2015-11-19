#include "pch.hpp"

#include "sequence_collection.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"


namespace PythonType
{


namespace Detail
{


template< typename T >
typename std::enable_if< Sequence< T >::can_resize, typename Sequence< T >::result_type >::type
insert( typename Sequence< T >::container_type & container_,
	const typename Sequence< T >::key_type i,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	auto noneType = PyScript::ScriptObject( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	typename Sequence< T >::key_type resultIndex = i;

	// Insert at start
	if (i <= 0)
	{
		// Only add 1 at start
		resultIndex = 0;
		const bool success = container_.insert( 0, noneType );
		if (!success)
		{
			return Sequence< T >::result_type( end, false );
		}
	}
	// Append to end
	else if (i >= container_.size())
	{
		// Only add 1 at end
		resultIndex = container_.size();
		const bool success = container_.append( noneType );
		if (!success)
		{
			return Sequence< T >::result_type( end, false );
		}
	}
	// Insert in middle
	else
	{
		// Add 1 in middle
		const bool success = container_.insert( i, noneType );
		if (!success)
		{
			return Sequence< T >::result_type( end, false );
		}
	}

	return Sequence< T >::result_type(
		std::make_shared< Sequence< T >::iterator_impl_type >( container_,
			resultIndex,
			typeConverters_ ),
		true );
}


template< typename T >
typename std::enable_if< !Sequence< T >::can_resize, typename Sequence< T >::result_type >::type
insert( typename Sequence< T >::container_type & container_,
	const typename Sequence< T >::key_type i,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	NGT_ERROR_MSG( "Cannot insert into container that does not resize\n" );
	return Sequence< T >::result_type( end, false );
}


} // namespace Detail


template< typename T >
Sequence< T >::Sequence(
	const typename Sequence< T >::container_type & container,
	const PythonTypeConverters & typeConverters )
	: CollectionImplBase()
	, container_( container )
	, typeConverters_( typeConverters )
{
}


template< typename T >
bool Sequence< T >::empty() const /* override */
{
	return (container_.size() == 0);
}


template< typename T >
size_t Sequence< T >::size() const /* override */
{
	return container_.size();
}


template< typename T >
CollectionIteratorImplPtr Sequence< T >::begin() /* override */
{
	const key_type startIndex = 0;
	return std::make_shared< iterator_impl_type >( container_,
		startIndex,
		typeConverters_ );
}


template< typename T >
CollectionIteratorImplPtr Sequence< T >::end() /* override */
{
	const key_type endIndex = container_.size();
	return std::make_shared< iterator_impl_type >( container_,
		endIndex,
		typeConverters_ );
}


template< typename T >
typename Sequence< T >::result_type Sequence< T >::get( const Variant & key,
	CollectionImplBase::GetPolicy policy ) /* override */
{
	key_type i;
	if (!key.tryCast( i ))
	{
		return result_type( this->end(), false );
	}

	// If you pass in a negative index,
	// Python adds the length of the sequence to the index.
	// E.g. list[-1] gets the last item in the list
	// SequenceIterator should always have an index in the range start-end
	if (i < 0)
	{
		i += container_.size();
	}

	if (policy == GET_EXISTING)
	{
		if (i < 0)
		{
			return result_type( this->end(), false );
		}
		if (i < container_.size())
		{
			return result_type(
				std::make_shared< iterator_impl_type >( container_,
					i,
					typeConverters_ ),
				false );
		}
		else
		{
			return result_type( this->end(), false );
		}
	}
	else if (policy == GET_NEW)
	{
		return Detail::insert< T >( container_, i, this->end(), typeConverters_ );
	}
	else if (policy == GET_AUTO)
	{
		const bool found = ((i >= 0) && (i < container_.size()));
		if (found)
		{
			// Get existing
			return result_type(
				std::make_shared< iterator_impl_type >( container_,
					i,
					typeConverters_ ),
				false );
		}

		// Insert new at start or end
		return Detail::insert< T >( container_, i, this->end(), typeConverters_ );
	}
	else
	{
		assert( false && "Not implemented" );
		return result_type( this->end(), false );
	}
}


template< typename T >
CollectionIteratorImplPtr Sequence< T >::erase(
	const CollectionIteratorImplPtr & pos ) /* override */
{
	return nullptr;
}


template< typename T >
size_t Sequence< T >::erase( const Variant & key ) /* override */
{
	return 0;
}


template< typename T >
CollectionIteratorImplPtr Sequence< T >::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last ) /* override */
{
	return nullptr;
}


template< typename T >
const TypeId & Sequence< T >::keyType() const /* override */
{
	return TypeId::getType< key_type >();
}


template< typename T >
const TypeId & Sequence< T >::valueType() const /* override */
{
	return TypeId::getType< value_type >();
}


template< typename T >
const TypeId & Sequence< T >::containerType() const /* override */
{
	return TypeId::getType< container_type >();
}


template< typename T >
void * Sequence< T >::containerData() const /* override */
{
	return const_cast< void * >( static_cast< const void * >( &container_ ) );
}


template< typename T >
bool Sequence< T >::canResize() const /* override */
{
	return can_resize;
}


// Explicit instantiations
template class Sequence< PyScript::ScriptList >;
template class Sequence< PyScript::ScriptSequence >;
template class Sequence< PyScript::ScriptTuple >;


} // namespace PythonType
