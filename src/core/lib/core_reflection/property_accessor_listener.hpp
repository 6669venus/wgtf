#ifndef PROPERTY_ACCESSOR_LISTENER_HPP
#define PROPERTY_ACCESSOR_LISTENER_HPP

#include "core_variant/collection.hpp"

class Variant;
class PropertyAccessor;
class ReflectedMethodParameters;

class PropertyAccessorListener
{
public:
	virtual void preSetValue( const PropertyAccessor & accessor, const Variant& value ) {}
	virtual void postSetValue( const PropertyAccessor & accessor, const Variant& value ) {}

	virtual void preInvoke(
		const PropertyAccessor & accessor, const ReflectedMethodParameters& parameters, bool undo ) {}
	virtual void postInvoke(
		const PropertyAccessor & accessor, Variant result, bool undo ) {}
};

#endif // PROPERTY_ACCESSOR_LISTENER_HPP
