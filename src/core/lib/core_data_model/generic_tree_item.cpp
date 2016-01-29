#include "generic_tree_item.hpp"

GenericTreeItem::GenericTreeItem()
	: model_( nullptr )
	, hidden_( false )
{
}

GenericTreeItem::~GenericTreeItem()
{
}

bool GenericTreeItem::empty() const
{
	return (this->size() == 0);
}
