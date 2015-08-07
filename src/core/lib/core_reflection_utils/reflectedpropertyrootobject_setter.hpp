#ifndef REFLECTED_PROPERTY_ROOTOBJECT_SETTER_HPP
#define REFLECTED_PROPERTY_ROOTOBJECT_SETTER_HPP

#include "core_reflection/object_handle.hpp"
#include "wg_types/event.hpp"

class IDefinitionManager;
class ReflectedPropertyRootObjectSetter
{
public:
	ReflectedPropertyRootObjectSetter(ObjectHandle & rootObject, IDefinitionManager * pDefManager);
	~ReflectedPropertyRootObjectSetter();
	void setValue( const ObjectHandle & data );
	const ObjectHandle & getValue() const;
	IDefinitionManager * getDefinitionManager() const;

	PUBLIC_EVENT( ReflectedPropertyRootObjectSetter, PreDataChanged )
	PUBLIC_EVENT( ReflectedPropertyRootObjectSetter, PostDataChanged )

private:
	ObjectHandle& source_;
	IDefinitionManager* pDefManager_;
};

#endif //REFLECTED_PROPERTY_ROOTOBJECT_SETTER_HPP