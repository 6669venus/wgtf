#include "reflectedpropertyrootobject_setter.hpp"

//==============================================================================
ReflectedPropertyRootObjectSetter::ReflectedPropertyRootObjectSetter(ObjectHandle & rootObject
																	, IDefinitionManager * pDefManager )
																	: source_(rootObject)
	, pDefManager_( pDefManager )

{
}

//==============================================================================
ReflectedPropertyRootObjectSetter::~ReflectedPropertyRootObjectSetter()
{}

//==============================================================================
void ReflectedPropertyRootObjectSetter::setValue( const ObjectHandle & data )
{
	this->notifyPreDataChanged();
	source_ = data;
	this->notifyPostDataChanged();
}

//==============================================================================
IDefinitionManager * ReflectedPropertyRootObjectSetter::getDefinitionManager() const
{
	return pDefManager_;
}

//==============================================================================
const ObjectHandle & ReflectedPropertyRootObjectSetter::getValue() const
{
	return source_;
}