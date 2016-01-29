#include "tree_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
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
	ObjectHandleT<TestStructure> testPage = pDefManager_->create< TestStructure >();
	testPage->init( defManager );
	objects.push_back( testPage );
	ObjectHandleT<TestInheritedStruct> testPage2 = pDefManager_->create< TestInheritedStruct >();
	testPage2->init( defManager );
	objects.push_back( testPage2 );
	testPage = pDefManager_->create< TestStructure >();
	testPage->init( defManager );
	objects.push_back( testPage );
	testPage2 = pDefManager_->create< TestInheritedStruct >();
	testPage2->init( defManager );
	objects.push_back( testPage2 );
	auto listModel = new VariantList();
	for (auto object : objects)
	{
		listModel->emplace_back( Variant(object) );
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

const IListModel * TreeListModel::getListModel() const
{
	return listModel_.get();
}

void TreeListModel::updateRootObject( const ObjectHandle & root )
{
	if (!root.isValid())
	{
		return;
	}
	treeRootObject_ = root;
}
