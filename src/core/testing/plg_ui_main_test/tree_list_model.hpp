#ifndef TREE_LIST_MODEL_HPP
#define TREE_LIST_MODEL_HPP

#include "core_reflection/object_handle.hpp"

class IComponentContext;
class IReflectionController;
class TreeListModel
{
public:
	TreeListModel();
	~TreeListModel();
	void init( IDefinitionManager & defManager, IReflectionController & controller );
	ObjectHandle getTreeModel() const;
	ObjectHandle getListModel() const;

private:
	IDefinitionManager* pDefManager_;
	IReflectionController* controller_;
	ObjectHandle listModel_;

};

#endif //TREE_LIST_MODEL_HPP
