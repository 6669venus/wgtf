#include "object_selection_helper.hpp"
#include "core_variant/variant.hpp"


ObjectSelectionHelper::ObjectSelectionHelper()
{

}

ObjectSelectionHelper::~ObjectSelectionHelper()
{

}


void ObjectSelectionHelper::init( const ObjectHandle & value )
{
	value_ = value;
}

Variant ObjectSelectionHelper::variantValue() const
{
	return Variant( value_ );
}


bool ObjectSelectionHelper::variantValue( const Variant& data )
{
	ObjectHandle typedValue;
	const bool ok = data.tryCast( typedValue );
	if (!ok)
	{
		return false;
	}

	this->value( typedValue );
	return true;
}

const ObjectHandle& ObjectSelectionHelper::value() const
{
	return value_;
}


void ObjectSelectionHelper::value( const ObjectHandle& data )
{
	if (value_ == data)
	{
		return;
	}
	this->notifyPreDataChanged();
	value_ = data;
	this->notifyPostDataChanged();
}