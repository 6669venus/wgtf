#ifndef I_REFLECTION_CONTROLLER_HPP
#define I_REFLECTION_CONTROLLER_HPP

#include "variant/variant.hpp"

class PropertyAccessor;

class IReflectionController
{
public:
	virtual Variant getValue( const PropertyAccessor & pa ) = 0;
	virtual void setValue( const PropertyAccessor & pa, const Variant & data ) = 0;
};

#endif//I_REFLECTION_CONTROLLER_HPP