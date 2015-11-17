#include "pch.hpp"

#include "list_collection.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"


namespace PythonType
{


namespace Detail
{


template< typename T >
typename std::enable_if< List< T >::can_resize,
	std::pair< CollectionIteratorImplPtr, bool > >::type
insert(
	typename List< T >::container_type & container_,
	const typename List< T >::key_type i,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	auto noneType = PyScript::ScriptObject( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	typename List< T >::key_type resultIndex = i;

	// Insert at start
	if (i <= 0)
	{
		// Only add 1 at start
		resultIndex = 0;
		const bool success = container_.insert( 0, noneType );
		if (!success)
		{
			return result_type( end, false );
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
			return result_type( end, false );
		}
	}
	// Insert in middle
	else
	{
		// Add 1 in middle
		const bool success = container_.insert( i, noneType );
		if (!success)
		{
			return result_type( end, false );
		}
	}

	return result_type(
		std::make_shared< List< T >::iterator_impl_type >( container_,
			resultIndex,
			typeConverters_ ),
		true );
}



template< typename T >
typename std::enable_if< !List< T >::can_resize,
	std::pair< CollectionIteratorImplPtr, bool > >::type
insert(
	typename List< T >::container_type & container_,
	const typename List< T >::key_type i,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;
	NGT_ERROR_MSG( "Cannot insert into container that does not resize" );
	return result_type( end, false );
}


} // namespace Detail


template< typename T >
List< T >::List( const typename List< T >::container_type & scriptList,
	const PythonTypeConverters & typeConverters )
	: CollectionImplBase()
	, container_( scriptList )
	, typeConverters_( typeConverters )
{
}


template< typename T >
bool List< T >::empty() const /* override */
{
	return (container_.size() == 0);
}


template< typename T >
size_t List< T >::size() const /* override */
{
	return container_.size();
}


template< typename T >
CollectionIteratorImplPtr List< T >::begin() /* override */
{
	const key_type startIndex = 0;
	return std::make_shared< iterator_impl_type >( container_,
		startIndex,
		typeConverters_ );
}


template< typename T >
CollectionIteratorImplPtr List< T >::end() /* override */
{
	const key_type endIndex = container_.size();
	return std::make_shared< iterator_impl_type >( container_,
		endIndex,
		typeConverters_ );
}


template< typename T >
std::pair< CollectionIteratorImplPtr, bool > List< T >::get( const Variant & key,
	CollectionImplBase::GetPolicy policy ) /* override */
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	key_type i;
	if (!key.tryCast( i ))
	{
		return result_type( this->end(), false );
	}

	// If you pass in a negative index,
	// Python adds the length of the list to the index.
	// E.g. list[-1] gets the last item in the list
	// ListIteratorImpl should always have an index in the range start-end
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
CollectionIteratorImplPtr List< T >::erase(
	const CollectionIteratorImplPtr & pos ) /* override */
{
	return nullptr;
}


template< typename T >
size_t List< T >::erase( const Variant & key ) /* override */
{
	return 0;
}


template< typename T >
CollectionIteratorImplPtr List< T >::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last ) /* override */
{
	return nullptr;
}


template< typename T >
const TypeId & List< T >::keyType() const /* override */
{
	static auto s_KeyType = TypeId::getType< key_type >();
	return s_KeyType;
}


template< typename T >
const TypeId & List< T >::valueType() const /* override */
{
	static auto s_ValueType = TypeId::getType< value_type >();
	return s_ValueType;
}


template< typename T >
bool List< T >::canResize() const /* override */
{
	return can_resize;
}


// Explicit instantiations
template class List< PyScript::ScriptList >;
template class List< PyScript::ScriptSequence >;
template class List< PyScript::ScriptTuple >;


} // namespace PythonType
