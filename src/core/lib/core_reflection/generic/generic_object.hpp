#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/generic/generic_property.hpp"
#include "reflection/property_accessor.hpp"
#include "variant/variant.hpp"
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


	Variant get( const char * name ) const;

	template< typename T>
	void set( const char * name, const T & value)
	{
		TypeId typeId = TypeId::getType< T >();
		Variant variantValue( value );
		setProperty( name, typeId, variantValue );
	}

private:
	GenericObject( const GenericObject & );
	const GenericObject & operator == ( const GenericObject & );

	mutable std::unordered_map< const GenericProperty *, Variant > properties_;

	friend class GenericProperty;
	void setProperty(
		const char * name, const TypeId & typeId, Variant & value ) const;

	const IClassDefinition * definition_;
};

#endif //GENERIC_OBJECT_HPP