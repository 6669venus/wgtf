#ifndef REFLECTION_UTILITIES_HPP
#define REFLECTION_UTILITIES_HPP

#include "../interfaces/i_class_definition.hpp"
#include "../i_definition_manager.hpp"
#include "../object_handle.hpp"
#include "core_variant/container_helper.hpp"

class ReflectedPolyStruct;
class PropertyAccessor;

namespace ReflectionUtilities
{

	template <typename To >
	const To * dynamicCast( const ReflectedPolyStruct & polyStruct );

	template <typename To >
	To * dynamicCast( ReflectedPolyStruct & polyStruct );

	template <typename To >
	To * dynamicCast( ReflectedPolyStruct * polyStruct );

	template <typename To, typename From >
	To * dynamicCast( void *, void *, From * );

	template <typename To, typename From >
	To * dynamicCast(	void * pObject, const ReflectedPolyStruct * typeIdentifier, From * fromType );

//==============================================================================
namespace internal
{
	template <typename Lookup>
	const IClassDefinition * getDefinitionImpl(
		const Lookup * object, const void *, IDefinitionManager & )
	{
		return NULL;
	}

	template <typename CollectionType, typename Lookup >
	const IClassDefinition * getDefinitionImpl(
		const CollectionType * object, const std::vector< Lookup > *, IDefinitionManager & definitionManager )
	{
		assert( object == NULL );
		Lookup * lookup = NULL;
		return getDefinitionImpl< Lookup >( lookup, lookup, definitionManager );
	}

	template <typename Lookup>
	const IClassDefinition * getDefinitionImpl(
		const Lookup * object, const ReflectedPolyStruct *, IDefinitionManager & definitionManager )
	{
		assert( object == NULL );
		return definitionManager.getDefinition< Lookup >();
	}


	template< typename BaseReflectedType >
	struct BaseReflectedTypeHelper
	{
		//======================================================================
		template< typename PointerType >
		static PointerType * safeCast( void * pAnon )
		{
			auto pPS = reinterpret_cast< BaseReflectedType * >( pAnon );
			if (pPS == nullptr)
			{
				return nullptr;
			}
			return dynamicCast< PointerType >( pPS );
		}


		//======================================================================
		template< typename PointerType >
		static const void * extractPointer( const PointerType * pKnown )
		{
			if (pKnown == nullptr)
			{
				return nullptr;
			}
			return reinterpret_cast< const void * >(
				dynamicCast< const BaseReflectedType >( *pKnown ) );
		}
	};


	//==========================================================================
	struct AnonTypeHelper
	{
		template< typename PointerType >
		static PointerType * safeCast( void * pointer )
		{
			return reinterpret_cast< PointerType * >( pointer );
		}

		//No processing needed just pass the pointer through
		template< typename PointerType >
		static const void * extractPointer( const PointerType * pKnown )
		{
			return reinterpret_cast< const void * >( pKnown );
		}
	};


	template< bool isPolyStruct >
	struct ReflectionCastHelper
		: public AnonTypeHelper {};

	//==========================================================================
	template<>
	struct ReflectionCastHelper< true >
		: public BaseReflectedTypeHelper< ReflectedPolyStruct >
	{
	};

	//==========================================================================
	template< typename TargetType >
	struct PointerHelper
	{
		//======================================================================
		static bool isPointerType( ... )
		{
			return std::is_pointer< TargetType >::value;
		}


		//======================================================================
		template< template < typename > class Container, typename FirstArgType >
		static auto isPointerType( const Container< FirstArgType > * pointer )
			-> typename std::conditional< std::is_same<
			decltype( std::declval< Container< FirstArgType > const >().get() ),
			decltype( std::declval< Container< FirstArgType > const >().get() ) >::value,
			bool,
			bool >::type
		{
			return true;
		}


		//======================================================================
		template< template < typename, typename > class Container, typename FirstArgType, typename SecondArgType >
		static auto isPointerType( const Container< FirstArgType, SecondArgType > * pointer )
			-> typename std::conditional< std::is_same<
			decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() ),
			decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() ) >::value,
			bool,
			bool >::type
		{
			return true;
		}

		//======================================================================
		template< typename T >
		static auto extractPointer( const T & pointer )
			-> decltype( extractPointerInt( pointer, pointer ) )
		{
			return extractPointerInt( pointer, pointer );
		}


		//======================================================================
		static const TargetType * extractPointerInt( ... )
		{
			return nullptr;
		}


		//======================================================================
		template< typename DummyArg >
		static const void * extractPointerInt(
			const TargetType * pointer, const DummyArg & t )
		{
			return
				ReflectionCastHelper<
				std::is_base_of< ReflectedPolyStruct, TargetType >::value
				>::template extractPointer< TargetType >( pointer );
		}


		//======================================================================
		template<
			template < typename >
				class Container,
					typename FirstArgType,
				typename DummyArg >
		static auto extractPointerInt(
			const Container< FirstArgType > & pointer, const DummyArg & t )
			-> typename std::conditional< std::is_same< decltype( std::declval< Container< FirstArgType > const >().get() ),
				const void * >::value,
				decltype(
					std::declval <
						//Recursive class helper
						PointerHelper<
						//Type of pointer returned
							typename std::remove_pointer<
							//Return type of pointer.get()
							decltype( std::declval< Container< FirstArgType > const >().get() )
						>::type >
					>().extractPointer( pointer.get() ) ),
				decltype(
					std::declval <
						//Recursive class helper
						PointerHelper<
						//Type of pointer returned
							typename std::remove_pointer<
							//Return type of pointer.get()
							decltype( std::declval< Container< FirstArgType > const >().get() )
						>::type >
					>().extractPointer( pointer.get() ) )
			>::type
		{
			auto pObj = pointer.get();
			typedef typename std::remove_pointer< decltype( pObj ) >::type ptr_type;
			return PointerHelper< ptr_type >::extractPointer( pObj );
		}

		//======================================================================
		template<
			template < typename, typename >
				class Container,
					typename FirstArgType,
					typename SecondArgType,
				typename DummyArg >
		static auto extractPointerInt(
			const Container< FirstArgType, SecondArgType > & pointer,
			const DummyArg & t )
			-> typename std::conditional< std::is_same< decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() ),
				const void * >::value,
				decltype(
					std::declval <
						//Recursive class helper
						PointerHelper<
							//Type of pointer returned
							typename std::remove_pointer<
								//Return type of pointer.get()
								decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() )
							>::type >
					>().extractPointer( pointer.get() ) ),
				decltype(
					std::declval <
						//Recursive class helper
						PointerHelper<
							//Type of pointer returned
							typename std::remove_pointer<
								//Return type of pointer.get()
								decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() )
						>::type >
					>().extractPointer( pointer.get() ) )
			>::type
		{
			auto pObj = pointer.get();
			typedef typename std::remove_pointer< decltype( pObj ) >::type ptr_type;
			return PointerHelper< ptr_type >::extractPointer( pObj );
		}


		//======================================================================
		template< typename PointerType >
		static PointerType * safeCastReflection( void * pointer )
		{
			return
				ReflectionCastHelper<
				std::is_base_of< ReflectedPolyStruct, PointerType >::value
				>::template safeCast< PointerType >( pointer );
		}


		//======================================================================
		static void setPointer( ... )
		{
			//Do nothing
		}


		//======================================================================
		static void setPointer( TargetType * & pointer, void * value )
		{
			pointer = safeCastReflection< TargetType >( value );
		}


		//======================================================================
		template<
			template < typename >
			class Container,
				typename FirstArgType >
		static auto setPointer(
			Container< FirstArgType > & pointer, void * value )
			-> typename std::conditional<
				std::is_same< decltype( std::declval< Container< FirstArgType > const >().get() ),
					void >::value, void, void >::type
		{
			typedef typename std::remove_pointer<
				decltype(
					std::declval< Container< FirstArgType > const >().get()
				) >::type ptr_type;
			pointer.reset( safeCastReflection< ptr_type >( value ) );
		}



		//======================================================================
		template<
			template < typename, typename >
		class Container,
			typename FirstArgType,
			typename SecondArgType >
		static auto setPointer(
			Container< FirstArgType, SecondArgType > & pointer, void * value )
			-> typename std::conditional<
				std::is_same< decltype( std::declval< Container< FirstArgType, SecondArgType > const >().get() ),
					void >::value, void, void >::type
		{
			typedef
				typename std::remove_pointer<
					decltype(
						std::declval< Container< FirstArgType, SecondArgType > const >().get()
					) >::type ptr_type;
			pointer.reset( safeCastReflection< ptr_type >( value ) );
		}
	};

	//======================================================================
	namespace BaseProviderGenerator
	{
		template< typename T >
		static ObjectHandle generate(
			const IDefinitionManager & defManager,
			const T & pImpl, const ReflectedPolyStruct & polyStruct )
		{
			return getPolyStructDefinition( &polyStruct )->getBaseProvider( &polyStruct );
		}


		template< typename T >
		static ObjectHandle generate(
			const IDefinitionManager & defManager,
			const T & pImpl, ... )
		{
			return ObjectHandle( pImpl, defManager.getDefinition< T >() );
		}
	};


	//==========================================================================
	template< typename To, bool isPolyStruct>
	struct CastHelper
	{
		//======================================================================
		static const To * dynamicCast( const ReflectedPolyStruct & polyStruct )
		{
			return nullptr;
		}
	};


	//==========================================================================
	template< typename To >
	struct CastHelper< To, true >
	{
		//======================================================================
		static const To * dynamicCast( const ReflectedPolyStruct & polyStruct )
		{
			auto srcDefinition = getPolyStructDefinition( &polyStruct );

			const auto * dstDefinition =
				srcDefinition->getDefinitionManager()->getDefinition< To >();

			if (dstDefinition != NULL &&
				srcDefinition->canBeCastTo(*dstDefinition))
			{
				return static_cast< const To * >( &polyStruct );
			}
			return NULL;
		}
	};
} // namespace internal



// =============================================================================
template <typename Lookup>
const IClassDefinition * getDefinition(
	const Lookup * object, IDefinitionManager & definitionManager )
{
	return internal::getDefinitionImpl(object, object, definitionManager );
}


bool isPolyStruct( const PropertyAccessor & pa );
bool isStruct( const PropertyAccessor & pa );

// =============================================================================
template< typename T >
bool isPointerType()
{
	T * typePointer = nullptr;
	return internal::PointerHelper< T >::isPointerType( typePointer );
}


// =============================================================================
template< typename T >
void * extractPointer( const T & value )
{
	return const_cast< void * >( reinterpret_cast< const void * >(
		internal::PointerHelper< typename std::remove_pointer< T >::type >::extractPointer( value ) ) );
}


// =============================================================================
template< typename T >
void setPointer( T & pointer, const void * value )
{
	internal::PointerHelper< typename std::remove_pointer< T >::type >::setPointer(
		pointer, const_cast< void * >( value ) );
}


//=============================================================================
template <typename To >
const To * dynamicCast( const ReflectedPolyStruct & polyStruct )
{
	return internal::CastHelper<
		To,
		std::is_base_of< ReflectedPolyStruct, To >::value
	>::dynamicCast( polyStruct );
}


//=============================================================================
template <typename To >
To * dynamicCast( ReflectedPolyStruct & polyStruct )
{
	return const_cast< To * >(
		internal::CastHelper<
			To,
			std::is_base_of< ReflectedPolyStruct, To >::value
		>::dynamicCast( polyStruct ) );
}


//=============================================================================
template <typename To >
To * dynamicCast( ReflectedPolyStruct * polyStruct )
{
	if (polyStruct == NULL)
	{
		return NULL;
	}
	return dynamicCast< To >( *polyStruct );
}


// =============================================================================
template <typename To, typename From >
To * dynamicCast( void *, void *, From * )
{
	return NULL;
}


// =============================================================================
template <typename To, typename From >
To * dynamicCast(
	void * pObject, const ReflectedPolyStruct * typeIdentifier, From * fromType )
{
	if (pObject == NULL)
	{
		return NULL;
	}
	return dynamicCast< To >(
		*(static_cast< ReflectedPolyStruct * >( pObject ) ) );
}


template< typename T >
struct PropertyTypeHelper
{
	static bool getType( TypeId & o_TypeId )
	{
		if (std::is_base_of<
			ReflectedPolyStruct, typename ContainerHelper< T >::FirstArgType >::value)
		{
			o_TypeId = TypeId::getType< typename ContainerHelper< T >::FirstArgType >();
			return true;
		}
		return false;
	}
};


// =============================================================================
template< typename T >
ObjectHandle generateBaseProvider(
	const T & pImpl,
	const IDefinitionManager & defManager )
{
	return internal::BaseProviderGenerator::generate(
		defManager,
		pImpl, pImpl );
}


// =============================================================================
template< typename T >
Variant copy( T & value )
{
	typedef typename Variant::traits< T >::storage_type variant_type;
	if ( Variant::typeIsRegistered< variant_type >() )
	{
		return Variant( value );
	}

	return ObjectHandle( value );
}


// =============================================================================
template< typename T >
Variant reference( T & value )
{
	typedef typename Variant::traits< T >::storage_type variant_type;
	if ( Variant::typeIsRegistered< variant_type >() )
	{
		return Variant( value );
	}

	return ObjectHandle( &value );
}


// =============================================================================
template<>
Variant copy< Variant >( Variant & value );


// =============================================================================
template<>
Variant copy< const Variant >( const Variant & value );


// =============================================================================
template<>
Variant reference< Variant >( Variant & value );


// =============================================================================
template<>
Variant reference< const Variant >( const Variant & value );


// =============================================================================
template< typename T >
bool extract( const Variant & variant, T & value, const IDefinitionManager & defManager )
{
	if (variant.isVoid())
	{
		return false;
	}

	if (variant.tryCast( value ))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast( handle ))
	{
		auto valuePtr = reflectedCast< T >( handle, defManager ).get();
		if (valuePtr)
		{
			value = *valuePtr;
			return true;
		}
	}

	return false;
}


// =============================================================================
template< typename T >
bool extract(const Variant & variant, ObjectHandleT< T > & value, const IDefinitionManager & defManager)
{
	if (variant.isVoid())
	{
		return false;
	}

	if (variant.tryCast( value ))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast( handle ))
	{
		value = reflectedCast< T >( handle, defManager );
		return true;
	}

	return false;
}


}
#endif //REFLECTION_UTILITIES_HPP
