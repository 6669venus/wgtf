#ifndef REFLECTED_TREE_MODEL_HPP
#define REFLECTED_TREE_MODEL_HPP

#include "data_model/generic_tree_model.hpp"
#include "data_model/reflection/reflected_object_item.hpp"


class IReflectionController;
class PropertyAccessorListener;

class ReflectedTreeModel
	: public GenericTreeModel
{
public:
	ReflectedTreeModel( const ObjectHandle & object,
		IReflectionController * controller );
	virtual ~ReflectedTreeModel();

private:
	ReflectedObjectItem rootItem_;
	std::shared_ptr< PropertyAccessorListener > listener_;
};

#endif