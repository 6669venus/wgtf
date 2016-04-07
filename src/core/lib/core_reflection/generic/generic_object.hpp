#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP


#include "base_generic_object.hpp"
#include "core_reflection/reflection_dll.hpp"


#include <unordered_map>

class GenericObject;
typedef ObjectHandleT<GenericObject> GenericObjectPtr;

class GenericDefinition;

class GenericProperty;
class TypeId;

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

/**
 *	GenericObject is an object that has a "generic type".
 *	And it has storage for any properties that are added/removed.
 */
class REFLECTION_DLL GenericObject : public BaseGenericObject
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

private:
	GenericObject( const GenericObject & );
	const GenericObject & operator == ( const GenericObject & );

	ObjectHandle getDerivedType() const override;
	ObjectHandle getDerivedType() override;

	mutable std::unordered_map< const GenericProperty *, Variant > properties_;
	friend class GenericProperty;
};

#pragma warning (pop)

#endif //GENERIC_OBJECT_HPP