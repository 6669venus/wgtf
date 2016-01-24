#include "base_property_with_metadata.hpp"

BasePropertyWithMetaData::BasePropertyWithMetaData( const IBasePropertyPtr & property, MetaHandle metaData )
	: property_( property )
	, metaData_( metaData )
{
	assert( property_ != nullptr );
}

BasePropertyWithMetaData::~BasePropertyWithMetaData()
{

}

const TypeId & BasePropertyWithMetaData::getType() const
{
	return property_->getType();
}

const char * BasePropertyWithMetaData::getName() const
{
	return property_->getName();
}

MetaHandle BasePropertyWithMetaData::getMetaData() const
{
	//TODO
	//return property_->getMetaData() + metaData_;
	return metaData_;
}

bool BasePropertyWithMetaData::readOnly() const
{
	return property_->readOnly();
}

bool BasePropertyWithMetaData::isMethod() const
{
	return property_->isMethod();
}

bool BasePropertyWithMetaData::isValue() const
{
	return property_->isValue();
}

bool BasePropertyWithMetaData::set( const ObjectHandle & handle,
				 const Variant & value,
				 const IDefinitionManager & definitionManager ) const
{
	return property_->set( handle, value, definitionManager );
}

Variant BasePropertyWithMetaData::get( const ObjectHandle & handle,
					const IDefinitionManager & definitionManager ) const
{
	return property_->get( handle, definitionManager );
}

Variant BasePropertyWithMetaData::invoke( const ObjectHandle & object,
					   const ReflectedMethodParameters & parameters )
{
	return property_->invoke( object, parameters );
}

size_t BasePropertyWithMetaData::parameterCount() const
{
	return property_->parameterCount();
}