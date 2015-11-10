#include "pch.hpp"

#include "list_collection.hpp"

#include "core_variant/variant.hpp"


namespace PythonType
{


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
		auto noneType = PyScript::ScriptObject( Py_None,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		if (i >= container_.size())
		{
			for (key_type j = 0; j < container_.size(); ++j)
			{
				const bool success = container_.append( noneType );
				if (!success)
				{
					return result_type( this->end(), false );
				}
			}
		}
		else
		{
			const bool success = container_.insert( i, noneType );
			if (!success)
			{
				return result_type( this->end(), false );
			}
		}

		return result_type(
			std::make_shared< iterator_impl_type >( container_,
				i,
				typeConverters_ ),
			true );
	}
	else if (policy == GET_AUTO)
	{
		const bool found = i < container_.size();
		if (!found)
		{
			auto noneType = PyScript::ScriptObject( Py_None,
				PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

			if (i >= container_.size())
			{
				for (key_type j = 0; j < container_.size(); ++j)
				{
					const bool success = container_.append( noneType );
					if (!success)
					{
						return result_type( this->end(), false );
					}
				}
			}
			else
			{
				const bool success = container_.insert( i, noneType );
				if (!success)
				{
					return result_type( this->end(), false );
				}
			}
		}

		return result_type(
			std::make_shared< iterator_impl_type >( container_,
				i,
				typeConverters_ ),
			!found );
	}
	else
	{
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
