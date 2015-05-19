#ifndef REFLECTED_PROPERTY_HPP
#define REFLECTED_PROPERTY_HPP

#include "base_property.hpp"
#include "utilities/reflection_utilities.hpp"

#include "variant/collection.hpp"
#include "variant/variant.hpp"

template< typename TargetType, typename BaseType >
class ReflectedProperty
	: public BaseProperty
{
public:	
	typedef ReflectedProperty< TargetType, BaseType > SelfType;
	typedef TargetType BaseType::* member_ptr;

	explicit ReflectedProperty(
		const char * name, member_ptr memPtr,
		const TypeId & type )
		: BaseProperty( name, type )
		, memberPtr_( memPtr )
	{
		TypeId typeId( "" );
		if (ReflectionUtilities::PropertyTypeHelper< TargetType >::getType( typeId ))
		{
			setType( typeId );
		}
	}


	//==========================================================================
	Variant get( const ObjectHandle & pBase ) const override
	{
		auto pObject = pBase.getBase< BaseType >();
		if (pObject && memberPtr_)
		{
			return pObject->*memberPtr_;
		}
		else
		{
			return Variant();
		}
	}


	//==========================================================================
	void set( const ObjectHandle & pBase, const Variant & value ) const override
	{
		set_Value< std::is_same<TargetType, Variant>::value >::set(
			pBase, memberPtr_, value );
	}

	
private:
	member_ptr memberPtr_;

	template<bool is_Variant>
	struct set_Value
	{
		static void set(
			const ObjectHandle & pBase,
			member_ptr memberPtr,
			const Variant & value )
		{
			auto pObject = pBase.getBase< BaseType >();
			if (pObject && memberPtr)
			{
				pObject->*memberPtr = value;
			}
		}
	};

	template<>
	struct set_Value<false>
	{
		static void set(
			const ObjectHandle & pBase,
			member_ptr memberPtr,
			const Variant & value )
		{
			set_impl< variant::traits< TargetType >::can_downcast >::set(
				pBase, memberPtr, value );
		}
	};


	template<bool can_set>
	struct set_impl
	{
		static void set(
			const ObjectHandle & pBase,
			member_ptr memberPtr,
			const Variant & value )
		{
			auto pObject = pBase.getBase< BaseType >();
			if (pObject && memberPtr)
			{
				value.tryCast(pObject->*memberPtr);
			}
		}
	};

	template<>
	struct set_impl<false>
	{
		static void set(
			const ObjectHandle & pBase,
			member_ptr,
			const Variant & )
		{
			// nop
		}
	};

};

#endif // REFLECTED_PROPERTY_HPP
