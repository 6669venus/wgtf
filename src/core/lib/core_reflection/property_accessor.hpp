#ifndef PROPERTY_ACCESSOR_HPP
#define PROPERTY_ACCESSOR_HPP

#include <string>
#include <memory>
#include "i_definition_manager.hpp"
#include "core_variant/collection.hpp"

#include "reflection_dll.hpp"

class TypeId;
class ObjectHandle;
class IBaseProperty;
class IClassDefinition;
class MetaBase;
class Variant;
class ReflectedMethodParameters;

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class REFLECTION_DLL PropertyAccessor
{
public:
	PropertyAccessor();
	PropertyAccessor( const PropertyAccessor & other );
	PropertyAccessor( PropertyAccessor && other );
	
	PropertyAccessor& operator = ( const PropertyAccessor & other );
	PropertyAccessor& operator = ( PropertyAccessor && other );

	bool isValid() const;

	const TypeId & getType() const;
	PropertyAccessor getParent() const;

	MetaHandle getMetaData() const;

	const char * getName() const;
	const IClassDefinition * getStructDefinition() const;

	// TODO: hide these accessors
	bool canGetValue() const;
	Variant getValue() const;

	bool canSetValue() const;
	bool setValue(const Variant & value ) const;
	bool setValueWithoutNotification( const Variant & value ) const;

	bool canInvoke() const;
	Variant invoke( const ReflectedMethodParameters & parameters ) const;
	void invokeUndoRedo( const ReflectedMethodParameters & parameters, Variant result, bool undo ) const;

	IBasePropertyPtr getProperty() const { return property_; }
	const ObjectHandle & getObject() const { return object_; }

	const ObjectHandle & getRootObject() const;
	const char * getFullPath() const;

	const IDefinitionManager * getDefinitionManager() const;
private:
	std::shared_ptr<PropertyAccessor> parentAccessor_;
	ObjectHandle			object_;
	IBasePropertyPtr		property_;

	ObjectHandle			rootObject_;
	std::string				path_;
	const IDefinitionManager *	definitionManager_;

	friend class ClassDefinition;

	PropertyAccessor(
		const IDefinitionManager * definitionManager,
		const ObjectHandle & rootObject, const char * path );
	void setObject( const ObjectHandle & object );
	void setBaseProperty( const IBasePropertyPtr & property );
	void setParent( const PropertyAccessor& parent );
};

#pragma warning (pop)

#endif // PROPERTY_ACCESSOR_HPP
