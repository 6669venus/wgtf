#ifndef I_CLASS_DEFINITION_HPP
#define I_CLASS_DEFINITION_HPP

#include "../property_iterator.hpp"
#include "../ref_object_id.hpp"

class ObjectHandle;
class IClassDefinitionDetails;
class IBaseProperty;
class IDefinitionManager;
class MetaBase;
class PropertyAccessor;
class ReflectedPolyStruct;
class TypeId;

class IClassDefinition
{
public:
	virtual ~IClassDefinition() {}
	virtual const IClassDefinitionDetails & getDetails() const = 0;
	virtual PropertyAccessor bindProperty(
		const char * name, const ObjectHandle & object ) const = 0;

	//TODO: Remove, only used by ClassDefinition itself
	virtual void bindPropertyImpl( const char * name, const ObjectHandle & pBase, 
		PropertyAccessor & o_PropertyAccessor ) const = 0;

	virtual IDefinitionManager * getDefinitionManager() const = 0;

	virtual bool isGeneric() const = 0;
	virtual IClassDefinition * getParent() const = 0;
	virtual bool canBeCastTo( const IClassDefinition & definition ) const = 0;
	virtual void * castTo( const IClassDefinition & definition, void * object ) const = 0;
	virtual void * upCast( void * object ) const = 0;

	// Range for all properties contained in this and its parents' definitions
	virtual PropertyIteratorRange allProperties() const = 0;
	// Range for only properties contain in this definition
	virtual PropertyIteratorRange directProperties() const = 0;
	virtual const SortedPropertyCollection & sortedProperties() const = 0;
	virtual PropertyIterator getPropertyIterator(
		PropertyIterator::IterateStrategy =
			PropertyIterator::ITERATE_PARENTS ) const = 0;
	virtual IBaseProperty * findProperty( const TypeId & propertyId ) const = 0;

	virtual const char * getName() const = 0;
	virtual const MetaBase * getMetaData() const = 0;
	virtual ObjectHandle create() const = 0;
	virtual ObjectHandle createManagedObject( const RefObjectId & id = RefObjectId::zero() ) const = 0;

	virtual ObjectHandle getBaseProvider(
		const ReflectedPolyStruct * polyStruct ) const = 0;
	virtual ObjectHandle getBaseProvider( const void * pThis ) const = 0;
};

#endif // I_CLASS_DEFINITION_HPP

