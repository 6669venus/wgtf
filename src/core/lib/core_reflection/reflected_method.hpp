#ifndef REFLECTED_METHOD_HPP
#define REFLECTED_METHOD_HPP

#include "base_property.hpp"
#include "core_variant/variant.hpp"

class ReflectedMethod
	: public BaseProperty
{
public:	
	explicit ReflectedMethod( const char * name )
		: BaseProperty( name, TypeId( "" ) )
	{}


	BasePropertyType propertyType() const override
	{
		return BasePropertyType::Method;
	}


	typedef ReflectedMethod SelfType;
};

#endif // REFLECTED_METHOD_HPP
