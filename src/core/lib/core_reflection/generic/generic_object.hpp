#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_variant/variant.hpp"
#include <unordered_map>

class GenericObject;
typedef ObjectHandleT<GenericObject> GenericObjectPtr;

class GenericDefinition;

/**
 *	GenericObject is an object that has a "generic type".
 *	
 *	Generic types are not based on a static class definition, like a C++ class.
 *	Generic types are more like script classes which may share the same
 *	definition or be per instance because they can dynamically
 *	add and remove members.
 */
class GenericObject : public DefinitionProvider
{
private:
	friend GenericDefinition;

public:
	/// Only GenericDefinition::create should use this function
	GenericObject()
	{
	}
	static GenericObjectPtr create(
		IDefinitionManager & definitionManager, 
		const RefObjectId & id = RefObjectId::zero(), 
		const char* classDefinitionName = nullptr );

	template< typename T >
	bool get( const char * name, T & value ) const
	{
		auto variant = getProperty( name );
		return ReflectionUtilities::extract( variant,
			value,
			*this->getDefinition().getDefinitionManager() );
	}

	template< typename T>
	void set( const char * name, const T & value )
	{
		TypeId typeId = TypeId::getType< T >();
		auto variantValue = ReflectionUtilities::reference( value );
		setProperty( name, typeId, variantValue );
	}

	void set( const char * name, const Variant & value )
	{
		setProperty( name, value.type()->typeId(), const_cast< Variant & >( value ) );
	}

private:
	GenericObject( const GenericObject & );
	const GenericObject & operator == ( const GenericObject & );

	mutable std::unordered_map< const GenericProperty *, Variant > properties_;

	friend class GenericProperty;
	Variant getProperty( const char * name ) const;
	void setProperty( const char * name, const TypeId & typeId, Variant & value ) const;
};

#endif //GENERIC_OBJECT_HPP