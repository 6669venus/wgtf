#ifndef I_CLASS_DEFINITION_DETAILS_HPP
#define I_CLASS_DEFINITION_DETAILS_HPP

#include <unordered_map>
#include <memory>

class ObjectHandle;

class IClassDefinition;
class IClassDefinitionModifier;
class IPropertyIteratorImpl;

class TypeId;

template<typename T> class ObjectHandleT;
class MetaBase;
typedef ObjectHandleT< MetaBase > MetaHandle;

/**
 *	Interface for providing inheritance info about a type.
 *	Add implementations for extracting info from types in different languages.
 *	E.g. C++ classes, QML objects, Python classes.
 */
class IClassDefinitionDetails
{
public:
	virtual ~IClassDefinitionDetails() {}

	/**
	 *	Check if this type is an interface or a concrete type.
	 *	@return true if the type is an interface.
	 */
	virtual bool isAbstract() const = 0;

	/**
	 *	Check if this type is a generic definition.
	 *	
	 *	A definition is generic if it is defined at runtime and does not
	 *	persist external to the application lifetime,
	 *	unlike a definition that has been built from a static type.
	 *	
	 *	The serializer needs this information to determine how the
	 *	definition is to be serialized.
	 *	
	 *	@return true if the type is generated at runtime.
	 */
	virtual bool isGeneric() const = 0;

	/**
	 *	Get the name of the type that this defines.
	 *	@return the name of the type. Should not be an empty string or null.
	 */
	virtual const char * getName() const = 0;

	/**
	 *	Get the name of the parent/base class.
	 *	@note does not support multiple inheritance.
	 *	@return the name of the parent/base class or null if there isn't one.
	 */
	virtual const char * getParentName() const = 0;
	virtual MetaHandle getMetaData() const = 0;
	virtual ObjectHandle create(
		const IClassDefinition & classDefinition ) const = 0;
	virtual void * upCast( void * object ) const = 0;

	virtual std::shared_ptr< IPropertyIteratorImpl > getPropertyIterator() const = 0;

	virtual IClassDefinitionModifier * getDefinitionModifier() const = 0;
};

#endif // I_CLASS_DEFINITION_DETAILS_HPP

