#include "pch.hpp"

#include "list_collection.hpp"

#include "core_variant/variant.hpp"


namespace PythonType
{


namespace Detail
{

std::pair< CollectionIteratorImplPtr, bool > insert(
	List::container_type & container_,
	const List::key_type i,
	CollectionIteratorImplPtr end,
	const PythonTypeConverters & typeConverters_ )
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	auto noneType = PyScript::ScriptObject( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	List::key_type resultIndex = i;

	// Insert at start
	if (i < 0)
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
		std::make_shared< List::iterator_impl_type >( container_,
			resultIndex,
			typeConverters_ ),
		true );
}


} // namespace Detail


List::List( const PyScript::ScriptList & scriptList,
	const PythonTypeConverters & typeConverters )
	: CollectionImplBase()
	, container_( scriptList )
	, typeConverters_( typeConverters )
{
}


bool List::empty() const
{
	return (container_.size() == 0);
}


size_t List::size() const
{
	return container_.size();
}


CollectionIteratorImplPtr List::begin()
{
	const key_type startIndex = 0;
	return std::make_shared< iterator_impl_type >( container_,
		startIndex,
		typeConverters_ );
}


CollectionIteratorImplPtr List::end()
{
	const key_type endIndex = container_.size();
	return std::make_shared< iterator_impl_type >( container_,
		endIndex,
		typeConverters_ );
}


std::pair< CollectionIteratorImplPtr, bool > List::get( const Variant & key,
	CollectionImplBase::GetPolicy policy )
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
		return Detail::insert( container_, i, this->end(), typeConverters_ );
	}
	else if (policy == GET_AUTO)
	{
		const bool found = ((i >= 0) && (i < container_.size()));
		if (!found)
		{
			return Detail::insert( container_, i, this->end(), typeConverters_ );
		}

		return result_type(
			std::make_shared< iterator_impl_type >( container_,
				i,
				typeConverters_ ),
			!found );
	}
	else
	{
		assert( false && "Not implemented" );
		return result_type( this->end(), false );
	}
}


CollectionIteratorImplPtr List::erase(
	const CollectionIteratorImplPtr & pos )
{
	return nullptr;
}


size_t List::erase( const Variant & key )
{
	return 0;
}


CollectionIteratorImplPtr List::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last )
{
	return nullptr;
}


const TypeId & List::keyType() const
{
	static auto s_KeyType = TypeId::getType< key_type >();
	return s_KeyType;
}


const TypeId & List::valueType() const
{
	static auto s_ValueType = TypeId::getType< value_type >();
	return s_ValueType;
}


} // namespace PythonType
