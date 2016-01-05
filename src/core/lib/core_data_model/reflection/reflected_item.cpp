#include "reflected_item.hpp"
#include "core_reflection/object_handle.hpp"


const IClassDefinition * ReflectedItem::getDefinition() const
{ 
	auto pDefinitionManager = this->getDefinitionManager();
	assert( pDefinitionManager != nullptr );

	auto pDefinition = this->getObject().getDefinition( *pDefinitionManager );

	// Definition must exist or the view will have strange, blank bits
	assert( pDefinition != nullptr );
	return pDefinition;
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
