#ifndef I_BASE_REFLECTED_PROPERTY_HPP
#define I_BASE_REFLECTED_PROPERTY_HPP

#include "i_method.hpp"
#include <cassert>
#include "core_variant/variant.hpp"

//TODO:
//add #include "i_class_member.hpp"
//inherit from IClassMember and make IMethod inherit from it as well.

class TypeId;
class ObjectHandle;
class MetaBase;
class Variant;
class IDefinitionManager;

class IBaseProperty: public IMethod
{
public:
	virtual ~IBaseProperty() {}
	virtual const TypeId & getType() const = 0;
	virtual const char * getName() const = 0;
	virtual const MetaBase * getMetaData() const = 0;

	//TODO: remove isMethod and add separate accessors to the class definition for properties and methods.
	virtual bool isMethod() const { return false; }

	virtual bool set( const ObjectHandle & handle, const Variant & value, const IDefinitionManager & definitionManager ) const
	{
		assert( !isMethod() );
		return false;
	}


	virtual Variant get( const ObjectHandle & handle, const IDefinitionManager & definitionManager ) const
	{
		assert( !isMethod() );
		return 0;
	}


	virtual Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters )
	{
		assert( isMethod() );
		return 0;
	}

	virtual size_t parameterCount() const
	{
		assert( isMethod() );
		return 0;
	}
};

#endif // I_BASE_REFLECTED_PROPERTY_HPP
