#ifndef REFLECTED_TREE_MODEL_HPP
#define REFLECTED_TREE_MODEL_HPP

#include "core_data_model/generic_tree_model.hpp"
#include "core_data_model/reflection/reflected_object_item.hpp"


class IDefinitionManager;
class IReflectionController;
class PropertyAccessorListener;

class ReflectedTreeModel
	: public GenericTreeModel
{
	typedef GenericTreeModel base;
public:
	ReflectedTreeModel( const ObjectHandle & object,
		IDefinitionManager & definitionManager,
		IReflectionController * controller );

	ReflectedTreeModel( const ObjectHandle & object,
		const char * propertyName,
		IDefinitionManager & definitionManager,
		IReflectionController * controller );

	virtual ~ReflectedTreeModel();
	
	virtual void addRootItem( GenericTreeItem * item ) override;
	virtual void removeRootItem( GenericTreeItem * item ) override;
private:
	std::unique_ptr< ReflectedItem > rootItem_;
	IDefinitionManager & definitionManager_;
	std::shared_ptr< PropertyAccessorListener > listener_;
};

#endif