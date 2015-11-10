#include "pch.hpp"

#include "sequence_converter.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace PythonType
{


namespace Detail
{


class ListIteratorImpl:
	public CollectionIteratorImplBase
{
public:
	typedef PyScript::ScriptList container_type;
	typedef container_type::size_type key_type;
	typedef Variant value_type;
	typedef ListIteratorImpl this_type;

	ListIteratorImpl( const container_type & container,
		key_type index,
		const PythonTypeConverters & typeConverters );

	const container_type & container() const;
	/**
	 *	In Python if you pass in a negative index,
	 *	Python adds the length of the list to the index.
	 *	E.g. list[-1] gets the last item in the list
	 *	For this implementation, negative indexes should be checked and
	 *	converted to a positive one in the range start-end.
	 */
	key_type index() const;
	Variant key() const override;
	Variant value() const override;
	bool setValue( const Variant & value ) const override;
	void inc() override;
	bool equals( const CollectionIteratorImplBase& that ) const override;
	CollectionIteratorImplPtr clone() const override;

private:
	container_type container_;
	key_type index_;
	const PythonTypeConverters & typeConverters_;
};


class List : public CollectionImplBase
{
public:
	typedef List base;
	typedef PyScript::ScriptList container_type;
	typedef container_type::size_type key_type;
	typedef Variant value_type;
	typedef List this_type;

	typedef ListIteratorImpl iterator_impl_type;

	List( const container_type & container,
		const PythonTypeConverters & typeConverters );

	bool empty() const override;
	size_t size() const override;

	CollectionIteratorImplPtr begin() override;
	CollectionIteratorImplPtr end() override;

	std::pair< CollectionIteratorImplPtr, bool > get( const Variant & key,
		CollectionImplBase::GetPolicy policy ) override;

	CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & pos ) override;
	size_t erase( const Variant & key ) override;
	CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr & first,
		const CollectionIteratorImplPtr& last ) override;

	const TypeId & keyType() const override;
	const TypeId & valueType() const override;

	container_type container_;
	const PythonTypeConverters & typeConverters_;
};


ListIteratorImpl::ListIteratorImpl( const container_type & container,
	ListIteratorImpl::key_type index,
	const PythonTypeConverters & typeConverters )
	: container_( container )
	, index_( index )
	, typeConverters_( typeConverters )
{
	// ListIteratorImpl does not support negative indexes
	assert( index_ > 0 );
}


const ListIteratorImpl::container_type & ListIteratorImpl::container() const
{
	return container_;
}


ListIteratorImpl::key_type ListIteratorImpl::index() const
{
	return index_;
}


Variant ListIteratorImpl::key() const
{
	return Variant( index_ );
}


Variant ListIteratorImpl::value() const
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


bool ListIteratorImpl::setValue( const Variant & value ) const
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


void ListIteratorImpl::inc()
{
	++index_;
}


bool ListIteratorImpl::equals( const CollectionIteratorImplBase& that ) const
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	if (!t)
	{
		return false;
	}

	return (container_ == t->container_) &&
		(index_ == t->index_);
}


CollectionIteratorImplPtr ListIteratorImpl::clone() const
{
	return std::make_shared< this_type >( *this );
}


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


} // namespace Detail


SequenceConverter::SequenceConverter( const PythonTypeConverters & typeConverters )
	: IConverter()
	, typeConverters_( typeConverters )
{
}


bool SequenceConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	if (!PyScript::ScriptList::check( inObject ))
	{
		return false;
	}
	PyScript::ScriptList scriptList( inObject.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	auto collectionHolder = std::make_shared< Detail::List >( scriptList,
		typeConverters_ );
	Collection collection( collectionHolder );
	outVariant = Variant( collection );
	return true;
}


bool SequenceConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< Collection >::storage_type >())
	{
		return false;
	}
	Collection value;
	const auto isCollection = inVariant.tryCast< Collection >( value );
	if (!isCollection)
	{
		return false;
	}

	const auto size = static_cast< PyScript::ScriptList::size_type >( value.size() );
	auto scriptList = PyScript::ScriptList::create( size );

	auto itr = value.cbegin();
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto variantItem = (*itr);
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters_.toScriptType(
			variantItem, scriptItem );
		if (!convertResult)
		{
			return false;
		}

		const bool setResult = scriptList.setItem( i, scriptItem );
		if (!setResult)
		{
			return false;
		}

		++itr;
	}

	outObject = scriptList;
	return true;
}


} // namespace PythonType
