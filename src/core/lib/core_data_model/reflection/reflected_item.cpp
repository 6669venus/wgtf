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

IReflectionController * ReflectedItem::getController() const 
{ 
	if (controller_ != nullptr)
	{
		return controller_;
	}
	if (parent_ != nullptr)
	{
		return parent_->getController();
	}
	return nullptr;
}

void ReflectedItem::setController( IReflectionController * controller )
{
	controller_ = controller;
}

IDefinitionManager * ReflectedItem::getDefinitionManager() const 
{ 
	if (definitionManager_ != nullptr)
	{
		return definitionManager_;
	}
	if (parent_ != nullptr)
	{
		return parent_->getDefinitionManager();
	}
	return nullptr;
}

void ReflectedItem::setDefinitionManager( IDefinitionManager * definitionManager )
{
	definitionManager_ = definitionManager;
}
