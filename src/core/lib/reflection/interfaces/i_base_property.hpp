#ifndef I_BASE_REFLECTED_PROPERTY_HPP
#define I_BASE_REFLECTED_PROPERTY_HPP

class TypeId;
class ObjectHandle;
class MetaBase;
class Variant;

class IBaseProperty
{
public:
	virtual ~IBaseProperty() {}
	virtual const TypeId & getType() const = 0;
	virtual const char * getName() const = 0;
	virtual const MetaBase * getMetaData() const = 0;
	virtual void set(
		const ObjectHandle & handle, const Variant & value ) const = 0;
	virtual Variant get( const ObjectHandle & handle ) const = 0;
};

#endif // I_BASE_REFLECTED_PROPERTY_HPP
