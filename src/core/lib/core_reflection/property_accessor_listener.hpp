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
		const PropertyAccessor & accessor, const ReflectedMethodParameters& parameters, bool undo ) {}

	virtual void preItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & pos, size_t count ) {}

	virtual void postItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) {}

	virtual void preItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) {}

	virtual void postItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & pos, size_t count ) {}

};

#endif // PROPERTY_ACCESSOR_LISTENER_HPP
