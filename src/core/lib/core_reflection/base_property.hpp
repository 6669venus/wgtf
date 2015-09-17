#ifndef BASE_REFLECTED_PROPERTY_HPP
#define BASE_REFLECTED_PROPERTY_HPP

#include "interfaces/i_base_property.hpp"
#include "core_variant/type_id.hpp"

class BaseProperty
	: public IBaseProperty
{
public:
	BaseProperty( const char * name, const TypeId & type );

	const TypeId & getType() const override;
	const char * getName() const override;

	const MetaBase * getMetaData() const override;

protected:
	void setType( const TypeId & type ); 
	void setName( const char * name );

private:
	const char *		name_;
	TypeId				type_;

	friend class PropertyAccessor;
};

#endif // BASE_REFLECTED_PROPERTY_HPP
