#ifndef TREE_LIST_MODEL_HPP
#define TREE_LIST_MODEL_HPP

#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_list_model.hpp"

class IComponentContext;
class IReflectionController;

class TreeListModel
{
public:
	TreeListModel();
	~TreeListModel();
	void init( IDefinitionManager & defManager, IReflectionController & controller );
	ObjectHandle getTreeModel() const;
	const IListModel * getListModel() const;
	void updateRootObject( const ObjectHandle & root );


private:
	IDefinitionManager* pDefManager_;
	IReflectionController* controller_;
	std::unique_ptr< IListModel > listModel_;
	ObjectHandle treeRootObject_;

};

#endif //TREE_LIST_MODEL_HPP
