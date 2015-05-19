#include "reflected_item.hpp"

const IClassDefinition * ReflectedItem::getDefinition() const 
{ 
	auto parent = getParent();
	if (parent == nullptr)
	{
		return nullptr;
	}
	return static_cast< const ReflectedItem * >( parent )->getDefinition();
}

IReflectionPropertySetter * ReflectedItem::getPropertySetter() const 
{ 
	if (propertySetter_ != nullptr)
	{
		return propertySetter_;
	}
	if (parent_ != nullptr)
	{
		return parent_->getPropertySetter();
	}
	return nullptr;
}

void ReflectedItem::setPropertySetter( IReflectionPropertySetter * propertySetter )
{
	propertySetter_ = propertySetter;
}
