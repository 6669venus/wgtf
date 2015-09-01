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
 *	GenericObject
 */
class GenericObject
{
private:
	friend GenericDefinition;
	GenericObject() {};

public:
	static GenericObjectPtr create(
		IDefinitionManager & definitionManager, 
		const RefObjectId & id = RefObjectId::zero(), 
		const char* classDefinitionName = nullptr );

	template< typename T >
	T get( const char * name ) const
	{
		auto variant = getProperty( name );
		T value;
		ReflectionUtilities::toValue( variant, value, *definition_->getDefinitionManager() );
		return value;
	}

	template< typename T>
	void set( const char * name, const T & value )
	{
		TypeId typeId = TypeId::getType< T >();
		auto variantValue = ReflectionUtilities::toVariant( &value );
		setProperty( name, typeId, variantValue );
	}

	void set( const char * name, const Variant & value )
	{
		setProperty( name, value.type()->typeId(), const_cast< Variant & >( value ) );
	}

	const IClassDefinition * getDefinition() const
	{
		return definition_;
	}

private:
	GenericObject( const GenericObject & );
	const GenericObject & operator == ( const GenericObject & );

	mutable std::unordered_map< const GenericProperty *, Variant > properties_;

	friend class GenericProperty;
	Variant getProperty( const char * name ) const;
	void setProperty( const char * name, const TypeId & typeId, Variant & value ) const;

	const IClassDefinition * definition_;
};

#endif //GENERIC_OBJECT_HPP