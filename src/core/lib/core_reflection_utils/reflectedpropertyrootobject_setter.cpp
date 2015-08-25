#include "reflectedpropertyrootobject_setter.hpp"

//==============================================================================
ReflectedPropertyRootObjectSetter::ReflectedPropertyRootObjectSetter(ObjectHandle & rootObject )
	: source_(rootObject)

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
