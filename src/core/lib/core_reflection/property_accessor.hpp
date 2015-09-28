#ifndef PROPERTY_ACCESSOR_HPP
#define PROPERTY_ACCESSOR_HPP

#include <string>
#include <memory>
#include "i_definition_manager.hpp"
#include "core_variant/collection.hpp"

class TypeId;
class ObjectHandle;
class IBaseProperty;
class IClassDefinition;
class MetaBase;
class Variant;
class ReflectedMethodParameters;

class PropertyAccessor
{
public:
	PropertyAccessor();
	PropertyAccessor( const PropertyAccessor & other );
	PropertyAccessor( PropertyAccessor && other );
	
	PropertyAccessor& operator = ( PropertyAccessor && other );

	bool isValid() const;

	const TypeId & getType() const;
	PropertyAccessor getParent() const;

	const MetaBase * getMetaData() const;

	const char * getName() const;
	const IClassDefinition * getStructDefinition() const;

	// TODO: hide these accessors
	Variant getValue() const;
	bool setValue(const Variant & value ) const;
	bool setValueWithoutNotification( const Variant & value ) const;
	Variant invoke( const ReflectedMethodParameters & parameters, bool undo = false ) const;

	IBaseProperty * getProperty() const { return property_; }
	const ObjectHandle & getObject() const { return object_; }

	const ObjectHandle & getRootObject() const;
	const char * getFullPath() const;

	void firePreItemsInserted( const Collection::ConstIterator & pos, size_t count ) const;
	void firePostItemsInserted( const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) const;
	void firePreItemsRemoved( const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) const;
	void firePostItemsRemoved( const Collection::ConstIterator & pos, size_t count ) const;

	const IDefinitionManager * getDefinitionManager() const;
private:
	ObjectHandle			object_;
	std::shared_ptr< IBaseProperty > sharedProperty_;
	IBaseProperty *			property_;

	ObjectHandle			rootObject_;
	std::string				path_;
	const IDefinitionManager *	definitionManager_;

	friend class ClassDefinition;

	PropertyAccessor(
		const IDefinitionManager * definitionManager,
		const ObjectHandle & baseProvider, const char * path );
	void setObject( const ObjectHandle & object );
	void setBaseProperty( IBaseProperty * property );
	void setBaseProperty( const std::shared_ptr< IBaseProperty > & property );
};

#endif // PROPERTY_ACCESSOR_HPP
