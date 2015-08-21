#ifndef REFLECTED_OBJECT_VARIANT_HPP
#define REFLECTED_OBJECT_VARIANT_HPP

#include "reflected_object.hpp"
#include "utilities/reflection_utilities.hpp"

// Variant upcast and downcast

// Upcast ReflectedPolyStruct-derived raw pointer.
inline ReflectedPolyStruct* variantTypeUpcast(ReflectedPolyStruct* v)
{
	return v;
}


// Upcast ReflectedPolyStruct-derived smart pointer.
// Type T matches if T::element_type is derived from ReflectedPolyStruct.
template<typename T>
typename std::enable_if<std::is_base_of<
	ReflectedPolyStruct, typename T::element_type>::value, ReflectedPolyStruct*>::type variantTypeUpcast(const T& v)
{
	return v.get();
}


// Safely downcast ReflectedPolyStruct* to any derived type.
template<typename T>
typename std::enable_if<std::is_base_of<
	ReflectedPolyStruct, T>::value, bool>::type variantTypeDowncast(T** v, ReflectedPolyStruct* storage)
{
	if(T* tmp = ReflectionUtilities::dynamicCast<T>(storage))
	{
		if(v)
		{
			*v = tmp;
		}
		return true;
	}

	return false;
}

namespace std
{
	template<typename T>
	typename enable_if<is_base_of<ReflectedPolyStruct, T>::value,
		ObjectHandle >::type upcast( const shared_ptr< T >& v )
	{
		auto definitionManager = v != nullptr
			? v->getDefinition().getDefinitionManager()
			:nullptr;
		if (definitionManager == nullptr)
		{
			return ObjectHandle();
		}

		auto polyStruct = dynamic_pointer_cast< ReflectedPolyStruct, T >( v );
		return ReflectionUtilities::generateBaseProvider(
			polyStruct, *definitionManager );
	}


	template<typename T>
	typename enable_if<is_base_of<ReflectedPolyStruct, T >::value,
		bool>::type downcast( shared_ptr< T >* v, const ObjectHandle & storage )
	{
		auto polyStructPtr = storage.getBase< shared_ptr< ReflectedPolyStruct > >();
		if (polyStructPtr == nullptr)
		{
			return false;
		}

		*v = dynamic_pointer_cast< T, ReflectedPolyStruct >( *polyStructPtr );
		return true;
	}
}

#endif // REFLECTED_OBJECT_VARIANT_HPP
