#ifndef FUNCTION_PROPERTY_HPP
#define FUNCTION_PROPERTY_HPP

#include "base_property.hpp"

#include "utilities/reflection_utilities.hpp"
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include <functional>

template< typename TargetType, typename BaseType >
class FunctionPropertySet
	: public BaseProperty
{
public:
	typedef void (BaseType::*SetterFunc)( const TargetType & );

	explicit FunctionPropertySet(
		const char * name, const TypeId & type, SetterFunc setterFunc )
		: BaseProperty( name, type )
		, setter_( setterFunc )
	{
		TypeId typeId( "" );
		if (ReflectionUtilities::PropertyTypeHelper< TargetType >::getType( typeId ))
		{
			setType( typeId );
		}
	}


	//==========================================================================
	bool set( const ObjectHandle & provider, const Variant & value ) const override
	{
		return set_Value< std::is_same<TargetType, Variant>::value >::set(
					provider, setter_, value ); 
	}


private:
	SetterFunc	setter_;

	template<bool is_Variant>
	struct set_Value
	{
		static bool set(
			const ObjectHandle & provider,
			SetterFunc setter,
			const Variant & value )
		{
			BaseType * pBase = provider.getBase< BaseType >();
			if(pBase == nullptr || setter == nullptr)
			{
				return false;
			}
			(pBase->*setter)( value );
			return true;
		}
	};

	template<>
	struct set_Value<false>
	{
		static bool set(
			const ObjectHandle & provider,
			SetterFunc setter,
			const Variant & value )
		{
			return set_impl< variant::traits< TargetType >::can_downcast >::set(
						provider, setter, value );
		}
	};

	template<bool can_set>
	struct set_impl
	{
		static bool set(
			const ObjectHandle & provider,
			SetterFunc setter,
			const Variant & value )
		{
			BaseType * pBase = provider.getBase< BaseType >();
			if(pBase == nullptr || setter == nullptr)
			{
				return false;
			}
			bool br = value.with< TargetType >([=](const TargetType & v)
			{
				(pBase->*setter)( v );
			});
			return br;
		}
	};

	template<>
	struct set_impl<false>
	{
		static bool set(
			const ObjectHandle &,
			SetterFunc,
			const Variant & )
		{
			// nop
			return false;
		}
	};

};

template< typename TargetType, typename BaseType, bool ByValue, bool ByArg >
class FunctionPropertyGet
	: public FunctionPropertySet< TargetType, BaseType >
{
};

template< typename TargetType, typename BaseType >
class FunctionPropertyGet< TargetType, BaseType, true, false >
	: public FunctionPropertySet< TargetType, BaseType >
{
public:
	typedef TargetType (BaseType::*GetterFunc)() const;

	explicit FunctionPropertyGet(
		const char * name,
		GetterFunc getterFunc,
		FunctionPropertySet< TargetType, BaseType >::SetterFunc setterFunc,
		const TypeId & type )
		: FunctionPropertySet< TargetType, BaseType >( name, type, setterFunc )
		, getterFunc_( getterFunc )
	{
	}


	//==========================================================================
	Variant get( const ObjectHandle & provider ) const override
	{
		auto pBase = provider.getBase< BaseType >();
		TargetType result = ( pBase->*getterFunc_ )();
		return result;
	}


private:
	GetterFunc getterFunc_;
};

template< typename TargetType, typename BaseType >
class FunctionPropertyGet< TargetType, BaseType, false, false >
	: public FunctionPropertySet< TargetType, BaseType >
{
public:
	typedef const TargetType & (BaseType::*GetterFunc)() const;

	explicit FunctionPropertyGet(
		const char * name,
		GetterFunc getterFunc,
		FunctionPropertySet< TargetType, BaseType >::SetterFunc setterFunc,
		const TypeId & type )
		: FunctionPropertySet< TargetType, BaseType >( name, type, setterFunc )
		, getterFunc_( getterFunc )
	{
	}


	//==========================================================================
	Variant get(
		const ObjectHandle & provider ) const override
	{
		auto pBase = provider.getBase< BaseType >();
		return ( pBase->*getterFunc_ )();
	}

private:
	GetterFunc getterFunc_;
};

template< typename TargetType, typename BaseType >
class FunctionPropertyGet< TargetType, BaseType, true, true >
	: public FunctionPropertySet< TargetType, BaseType >
{
public:
	typedef void (BaseType::*GetterFunc)( TargetType * ) const;

	explicit FunctionPropertyGet(
		const char * name,
		GetterFunc getterFunc,
		FunctionPropertySet< TargetType, BaseType >::SetterFunc setterFunc,
		const TypeId & type )
		: FunctionPropertySet< TargetType, BaseType >( name, type, setterFunc )
		, getterFunc_( getterFunc )
	{
	}

	//==========================================================================
	Variant get(
		const ObjectHandle & provider ) const override
	{
		return CollectionHelper<>::get( provider, getterFunc_ );
	}
	
private:
	GetterFunc getterFunc_;

	template<
		bool IsCollection =
		!std::is_same< details::CollectionImpl< TargetType >::type, void >::value > 
	struct CollectionHelper
	{
		static Variant get(
			const ObjectHandle & provider,
			GetterFunc getterFunc )
		{
			auto pBase = provider.getBase< BaseType >();
			TargetType dummyRef;
			( pBase->*getterFunc )( &dummyRef );
			return dummyRef;
		}
	};


	template<>
	struct CollectionHelper< true >
	{
		static Variant get(
			const ObjectHandle & provider,
			GetterFunc getterFunc )
		{
			auto pBase = provider.getBase< BaseType >();
			auto pImpl = std::make_shared< CollectionHolder< TargetType > >();
			Collection collection( pImpl );
			( pBase->*getterFunc )( &pImpl->storage() );
			return collection;
		}
	};
};

template< typename TargetType, typename BaseType, bool ByValue, bool ByArg >
class FunctionProperty
	: public FunctionPropertyGet< TargetType, BaseType, ByValue, ByArg >
{
public:
	explicit FunctionProperty(
		const char * name,
		typename FunctionPropertyGet< TargetType, BaseType, ByValue, ByArg >::GetterFunc getterFunc,
		FunctionPropertySet< TargetType, BaseType >::SetterFunc setterFunc,
		const TypeId & type )
		: FunctionPropertyGet< TargetType, BaseType, ByValue, ByArg >( name, getterFunc, setterFunc, type )
	{
	}
};

template< typename TKey, typename TValue >
class FunctionCollectionIteratorImpl
	: public CollectionIteratorImplBase
{
public:
	typedef std::function< TKey & ( size_t index ) >	GetKeyFunc;
	typedef std::function< TValue & ( size_t index ) >	GetValueFunc;
	typedef FunctionCollectionIteratorImpl< TKey, TValue > this_type;

private:
	GetValueFunc	getValueFunc_;
	GetKeyFunc		getKeyFunc_;
	size_t			index_;

public:
	FunctionCollectionIteratorImpl(
		GetKeyFunc getKeyFunc,
		GetValueFunc getValueFunc,
		size_t index )
		: getValueFunc_( getValueFunc )
		, index_( index )
	{
	}


	//==========================================================================
	Variant key() const
	{
		if (getKeyFunc_)
		{
			return getKeyFunc_( index_ );
		}
		return index_;
	}


	//==========================================================================
	Variant value() const
	{
		return getValueFunc_( index_ );
	}


	//==========================================================================
	bool setValue( const Variant & v ) const
	{
		TValue & value = getValueFunc_( index_ );
		return v.tryCast( value );
	}


	//==========================================================================
	void inc()
	{
		index_++;
	}


	//==========================================================================
	bool equals( const CollectionIteratorImplBase & that) const
	{
		const this_type * pThis = dynamic_cast< const this_type * >( &that );
		if(pThis == nullptr)
		{
			return false;
		}
		return index_ == pThis->index_;
	}


	//==========================================================================
	CollectionIteratorImplPtr clone() const
	{
		return std::make_shared< this_type >( *this );
	}
};


template< typename TKey, typename TValue >
class FunctionCollection
	: public CollectionImplBase
{
public:
	typedef std::function< size_t () > GetSizeFunc;
	typedef std::function< size_t ( const TKey & ) > AddKeyFunc;
	typedef FunctionCollectionIteratorImpl< TKey, TValue > iterator_impl_type;

	template<
		typename TSizeFunc,
		typename TKeyFunc,
		typename TValueFunc,
		typename TAddKeyFunc >
	explicit FunctionCollection(
		TSizeFunc & getSizeFunc,
		TKeyFunc & getKeyFunc,
		TValueFunc & getValueFunc,
		TAddKeyFunc & addKeyFunc )
		: getSizeFunc_( getSizeFunc )
		, getKeyFunc_( getKeyFunc )
		, getValueFunc_( getValueFunc )
		, addKeyFunc_( addKeyFunc )
	{
	}


	//==========================================================================
	const TypeId & keyType() const override
	{
		static auto s_KeyType = TypeId::getType< TKey >();
		return s_KeyType;
	}


	//==========================================================================
	const TypeId & valueType() const override
	{
		static auto s_ValueType = TypeId::getType< TValue >();
		return s_ValueType;
	}


	//==========================================================================
	bool empty() const
	{
		return (this->size() == 0);
	}


	//==========================================================================
	size_t size() const
	{
		return getSizeFunc_();
	}

	//==========================================================================
	CollectionIteratorImplPtr begin()
	{
		return generateIterator( 0 );
	}


	//==========================================================================
	CollectionIteratorImplPtr end()
	{
		return generateIterator( size() );
	}


	//==========================================================================
	std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy)
	{
		typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

		TKey i;
		if(!key.tryCast(i))
		{
			return result_type(end(), false);
		}

		size_t index = 0;
		if (getKeyFunc_)
		{
			index = getKeyFunc_( i );
		}
		else
		{
			index = i;
		}
		switch(policy)
		{
		case GET_EXISTING:
			if(index < size())
			{
				return result_type( generateIterator( index ), false);
			}
			else
			{
				return result_type(end(), false);
			}

		case GET_NEW:
			index = addKeyFunc_( i );
			return result_type( generateIterator( index ), true);

		case GET_AUTO:
			{
				bool found = index < size();
				if(!found)
				{
					index = addKeyFunc_( i );
				}

				return result_type(
					generateIterator( index ), !found);
			}

		default:
			return result_type(end(), false);
		}
	}


	//==========================================================================
	CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr & pos )
	{
		return end();
	}


	//==========================================================================
	size_t erase( const Variant& key )
	{
		return size();
	}


	//==========================================================================
	CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & first, const CollectionIteratorImplPtr & last )
	{
		return end();
	}


private:
	CollectionIteratorImplPtr generateIterator( size_t index ) const
	{
		return std::make_shared< iterator_impl_type >( getKeyFunc_, getValueFunc_, index );
	}

	GetSizeFunc									getSizeFunc_;
	AddKeyFunc									addKeyFunc_;
	typename iterator_impl_type::GetValueFunc	getValueFunc_;
	typename iterator_impl_type::GetKeyFunc		getKeyFunc_;
};

template< typename TKey, typename TValue, typename BaseType >
class FunctionCollectionProperty
	: public BaseProperty
{
	typedef size_t (BaseType::*GetSizeFunc)() const;
	typedef size_t (BaseType::*AddKeyFunc )( const TKey & );
	typedef TValue & (BaseType::*GetValueFunc)( size_t ) const;
	typedef TKey & (BaseType::*GetKeyFunc )( size_t ) const;
public:
	explicit FunctionCollectionProperty(
		const char * name,
		GetSizeFunc getSizeFunc,
		GetKeyFunc getKeyFunc,
		GetValueFunc getValueFunc,
		AddKeyFunc addKeyFunc )
		: BaseProperty( name, TypeId::getType< FunctionCollectionProperty >() )
		, getSizeFunc_( getSizeFunc )
		, getValueFunc_( getValueFunc )
		, getKeyFunc_( getKeyFunc )
		, addKeyFunc_( addKeyFunc )
	{
	}

	//==========================================================================
	Variant get( const ObjectHandle & provider ) const override
	{
		auto pBase = provider.getBase< BaseType >();
		return Collection(
			std::make_shared< FunctionCollection< TKey, TValue > >(
				std::bind( getSizeFunc_, pBase ),
				getKeyFunc_ 
					? std::bind( getKeyFunc_, pBase, std::placeholders::_1 )
					: FunctionCollection< TKey, TValue >::iterator_impl_type::GetKeyFunc(),
				std::bind( getValueFunc_, pBase, std::placeholders::_1 ),
				std::bind( addKeyFunc_, pBase, std::placeholders::_1 ) )
		);
	}


	//==========================================================================
	bool set( const ObjectHandle & , const Variant & value ) const override
	{
		assert( false && "Cannot set." );
		return false;
	}


private:
	GetSizeFunc		getSizeFunc_;
	AddKeyFunc		addKeyFunc_;
	GetValueFunc	getValueFunc_;
	GetKeyFunc		getKeyFunc_;
};

#endif // FUNCTION_PROPERTY_HPP
