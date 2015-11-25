#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP


#include "base_generic_object.hpp"


#include <unordered_map>

class GenericObject;
typedef ObjectHandleT<GenericObject> GenericObjectPtr;

class GenericDefinition;

class GenericProperty;
class TypeId;


/**
 *	GenericObject is an object that has a "generic type".
 *	And it has storage for any properties that are added/removed.
 */
class GenericObject : public BaseGenericObject
{
public:
	/// Only GenericDefinition::create should use this function
	GenericObject()
	{
	}
	static GenericObjectPtr create(
		IDefinitionManager & definitionManager, 
		const RefObjectId & id = RefObjectId::zero(), 
		const char* classDefinitionName = nullptr );

	size_t getPropertyCount() const;

private:
	GenericObject( const GenericObject & );
	const GenericObject & operator == ( const GenericObject & );

	IBaseProperty * addProperty( const char * name,
		const TypeId & typeId,
		const MetaBase * pMetaBase ) override;

	ObjectHandle getDerivedType() const override;
	ObjectHandle getDerivedType() override;

	mutable std::unordered_map< const GenericProperty *, Variant > properties_;
	friend class GenericProperty;
};

#endif //GENERIC_OBJECT_HPP