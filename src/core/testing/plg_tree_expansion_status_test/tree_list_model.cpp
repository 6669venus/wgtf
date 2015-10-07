#include "tree_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "pages/test_page.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
//#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include <codecvt>


TreeListModel::TreeListModel()
{
}



TreeListModel::~TreeListModel()
{
}

void TreeListModel::init( IDefinitionManager & defManager, IReflectionController & controller )
{
	pDefManager_ = &defManager;
	assert( pDefManager_ != nullptr );
	controller_ = &controller;
	assert( controller_ != nullptr );
	std::vector< ObjectHandle > objects;
	ObjectHandleT<TestPage> testPage = pDefManager_->create< TestPage >();
	testPage->init();
	objects.push_back( testPage );
	ObjectHandleT<TestPage2> testPage2 = pDefManager_->create< TestPage2 >();
	testPage2->init( defManager );
	objects.push_back( testPage2 );
	testPage = pDefManager_->create< TestPage >();
	testPage->init();
	objects.push_back( testPage );
	testPage2 = pDefManager_->create< TestPage2 >();
	testPage2->init( defManager );
	objects.push_back( testPage2 );
	auto listModel = new VariantList();
	for (auto object : objects)
	{
		listModel->emplace_back( object );
		if (listModel->size() == 1)
		{
			treeRootObject_ = object;
		}
	}
	listModel_ = std::unique_ptr<IListModel>( listModel );
}


ObjectHandle TreeListModel::getTreeModel() const
{
	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( treeRootObject_, *pDefManager_, controller_ ) );
	return std::move( model );
}

ObjectHandle TreeListModel::getListModel() const
{
	return listModel_;
}

void TreeListModel::updateRootObject( const ObjectHandle & root )
{
	if (!root.isValid())
	{
		return;
	}
	treeRootObject_ = root;
}
