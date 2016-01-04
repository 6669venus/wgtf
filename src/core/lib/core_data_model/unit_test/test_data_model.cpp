#include "pch.hpp"

#include "test_data_model_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_unit_test/unit_test.hpp"

//---------------------------------------------------------------------------
// List Model Tests
//---------------------------------------------------------------------------
TEST_F( TestFixture, refreshFilteredList )
{
	initialise( TestStringData::STATE_LIST );
	VariantList & list = testStringData_.getVariantList();
	CHECK( list.size() > 0 );

	filter_.setRole( ValueRole::roleId_ );
	filteredTestList_.setSource( &list );
	filteredTestList_.setFilter( &filter_ );

	bool result = true;

	// Two items should be in the list
	filter_.setFilterText( "apple" );
	filteredTestList_.refresh( true );
	CHECK( filteredTestList_.size() == 2 );
	result = findItemInFilteredList( "pineapple" );
	CHECK( result == true );
	result = findItemInFilteredList( "orange" );
	CHECK( !result );
		
	// One item should be in the list
	filter_.setFilterText( "orange" );
	filteredTestList_.refresh( true );
	CHECK( filteredTestList_.size() == 1 );
	result = findItemInFilteredList( "apple" );
	CHECK( !result );

	// No items should be in the list
	filter_.setFilterText( "noitemfound" );
	filteredTestList_.refresh( true );
	result = findItemInFilteredList( "apple" );
	CHECK( !result );
	result = findItemInFilteredList( "orange" );
	CHECK( !result );
}

TEST_F( TestFixture, insertIntoListModel )
{
	initialise( TestStringData::STATE_LIST );
	VariantList & list = testStringData_.getVariantList();
	CHECK( list.size() > 0 );
	
	filter_.setRole( ValueRole::roleId_ );
	filteredTestList_.setSource( &list );
	filteredTestList_.setFilter( &filter_ );

	bool result = true;

	// Set the filter
	filter_.setFilterText( "berry" );
	filteredTestList_.refresh( true );

	// Insert an item that matches the filter and verify its presence as well as index location
	insertIntoListAtIndex( 3, "strawberry" );
	result = findItemInFilteredList( "strawberry" );
	CHECK( result == true );
	result = verifyListItemPosition( 3, "strawberry" );
	CHECK( result == true );

	// Insert an item that does not match the filter, but still verify its insert position
	insertIntoListAtIndex( 8, "watermelon" );
	result = findItemInFilteredList( "watermelon" );
	CHECK( !result );
	result = verifyListItemPosition( 8, "watermelon" );
	CHECK( result == true );

	// Insert an item to the end of the list that matches the filter and another that doesn't
	list.insert( list.end(), "blueberry" );
	result = findItemInFilteredList( "blueberry" );
	CHECK( result == true );
	result = verifyListItemPosition( static_cast< unsigned int >( list.size() ) - 1, "blueberry" );
	CHECK( result == true );

	list.insert( list.end(), "bamboo" );
	result = findItemInFilteredList( "bamboo" );
	CHECK( !result );
	result = verifyListItemPosition( static_cast< unsigned int >( list.size() ) - 1, "bamboo" );
	CHECK( result == true );
}

TEST_F( TestFixture, removeFromListModel )
{
	initialise( TestStringData::STATE_LIST );
	VariantList & list = testStringData_.getVariantList();
	CHECK( list.size() > 0 );
	
	filter_.setRole( ValueRole::roleId_ );
	filteredTestList_.setSource( &list );
	filteredTestList_.setFilter( &filter_ );

	std::string nextIndexValue;
	bool result = true;

	// Set the filter
	filter_.setFilterText( "apple" );
	filteredTestList_.refresh( true );

	// Remove an item from the front of the list (this will be "apple" and "pineapple" should still remain in the
	// list despite "apple" being removed)	
	getListItemValueAtIndex( 1, nextIndexValue );
	removeFromListAtIndex( 0 );
	result = findItemInFilteredList( "apple", true );
	CHECK( !result );
	result = findItemInFilteredList( "apple" ); // should find pineapple without exact match indicated
	CHECK( result == true );
	result = verifyListItemPosition( 0, "apple" );
	CHECK( !result );
	result = verifyListItemPosition( 0, nextIndexValue.c_str() );
	CHECK( result == true );

	// Remove from the middle of the list
	getListItemValueAtIndex( 10, nextIndexValue );
	removeFromListAtIndex( 9 ); // "jam"
	result = findItemInFilteredList( "jam", true );
	CHECK( !result );
	result = verifyListItemPosition( 9, "jam" );
	CHECK( !result );
	result = verifyListItemPosition( 9, nextIndexValue.c_str() );
	CHECK( result == true );

	// Remove twice from the back of the list
	unsigned int end;
	std::string endItemValue;

	for (unsigned int i = 0; i < 2; ++i)
	{
		end = static_cast< unsigned int >( list.size() ) - 1;
		getListItemValueAtIndex( end, endItemValue ); 

		removeFromListAtIndex( end );		
		end = static_cast< unsigned int >( list.size() ) - 1;

		result = findItemInFilteredList( endItemValue.c_str(), true );
		CHECK( !result );

		result = verifyListItemPosition( end, endItemValue.c_str() );
		CHECK( !result );
	}
}

TEST_F( TestFixture, changeListItem )
{
	initialise( TestStringData::STATE_LIST );
	VariantList & list = testStringData_.getVariantList();
	CHECK( list.size() > 0 );
	
	filter_.setRole( ValueRole::roleId_ );
	filteredTestList_.setSource( &list );
	filteredTestList_.setFilter( &filter_ );

	bool result = true;
	size_t oldListSize;

	// Set the filter
	filter_.setFilterText( "apple" );
	filteredTestList_.refresh( true );

	// In the filtered list before and after the change
	oldListSize = filteredTestList_.size();
	updateListItemAtIndex( 0, "apple pie" );
	result = ( filteredTestList_.size() == oldListSize );
	CHECK( result == true );

	// Not in the filtered list, but is after the change
	// Change "eggs" to "red apple" and make sure it appears in the filtered list
	oldListSize = filteredTestList_.size();
	updateListItemAtIndex( 4, "red apple" );
	result = ( filteredTestList_.size() > oldListSize );
	CHECK( result == true );
	result = verifyListItemPosition( 4, "eggs" );
	CHECK( !result );
	result = verifyListItemPosition( 4, "red apple" );
	CHECK( result == true );

	// Not in the filtered list before or after the change
	// Change "igloo" to "iguana" and make sure it doesn't appear in the filtered list
	oldListSize = filteredTestList_.size();
	updateListItemAtIndex( 8, "iguana" );
	result = ( filteredTestList_.size() == oldListSize );
	CHECK( result == true );
	result = verifyListItemPosition( 8, "iguana" );
	CHECK( result == true );
	
	// In the filtered list, but not after the change
	// Change "apple pie" to "waffle" and make sure it is not in the filtered list
	oldListSize = filteredTestList_.size();
	updateListItemAtIndex( 15, "waffle" );
	result = ( filteredTestList_.size() < oldListSize );
	CHECK( result == true );
	result = verifyListItemPosition( 15, "waffle" );
	CHECK( result == true );
}

//---------------------------------------------------------------------------
// Tree Model Tests
//---------------------------------------------------------------------------

TEST_F( TestFixture, refreshFilteredTree )
{
	initialise( TestStringData::STATE_TREE );
	UnitTestTreeModel & tree = testStringData_.getTreeModel();
	CHECK( tree.size( nullptr ) > 0 );

	filteredTestTree_.setSource( &tree );
	filteredTestTree_.setFilter( &filter_ );

	size_t size;
	bool result = true;

	// One item should be in the root node with corresponding children
	{
		filter_.setFilterText( "anim" );
		filteredTestTree_.refresh( true );

		// Only one item should remain
		CHECK( filteredTestTree_.size( nullptr ) == 1 );

		// This item should be "Animations"
		auto remainingItem = filteredTestTree_.item( 0, nullptr );
		CHECK( remainingItem != nullptr );
		result = verifyTreeItemMatch( remainingItem, "Animations", true );
		CHECK( result == true );

		// It should have 3 children indexed under the "anim" filter.
		CHECK( filteredTestTree_.size( remainingItem ) == 3 );
	}

	// No items should be in the tree after it has been filtered with a non-matching term
	{
		filter_.setFilterText( "unknown" );
		filteredTestTree_.refresh( true );

		// This should now be an empty tree
		CHECK( filteredTestTree_.size( nullptr ) == 0 );
	}

	// Multiple items in multiple root nodes should remain
	{
		filter_.setFilterText( "mo" ); // Animations/Monsters/anim_mouse and Models
		filteredTestTree_.refresh( true );

		// This should have at least two root items
		size = filteredTestTree_.size( nullptr );
		CHECK( size == 2 );
		if (size != 2)
		{
			FAIL( "Incorrect number of items found after filtering on 'mod'. Expected: 2" );
		}

		// The items should be "Animations" and "Models"
		auto firstItem = filteredTestTree_.item( 0, nullptr );
		CHECK( firstItem != nullptr );
		result = verifyTreeItemMatch( firstItem, "Animations", true );
		CHECK( result == true );

		auto secondItem = filteredTestTree_.item( 1, nullptr );
		CHECK( secondItem != nullptr );
		result = verifyTreeItemMatch( secondItem, "Models", true );
		CHECK( result == true );
	}

	// Children of items should remain alongside their parents, but none of the other children will be present
	{
		filter_.setFilterText( "fancy" );
		filteredTestTree_.refresh( true );

		// This should still have two root items
		size = filteredTestTree_.size( nullptr );
		CHECK( size == 1 );
		if (size != 1)
		{
			FAIL( "Incorrect number of items found after filtering on 'fancy'. Expected: 1" );
		}

		// The item should only have 1 child
		auto firstItem = filteredTestTree_.item( 0, nullptr );
		CHECK( firstItem != nullptr );
		CHECK( filteredTestTree_.size( firstItem ) == 1 );
	}
}

TEST_F( TestFixture, insertIntoTreeModel )
{
	initialise( TestStringData::STATE_TREE );
	UnitTestTreeModel & tree = testStringData_.getTreeModel();
	CHECK( tree.size( nullptr ) > 0 );

	filteredTestTree_.setSource( &tree );
	filteredTestTree_.setFilter( &filter_ );

	UnitTestTreeItem * item;
	std::string dataValue;
	bool result = true;

	filter_.setFilterText( "model" );
	filteredTestTree_.refresh( true );

	// Insert an item that should be included in the filtered tree
	{
		// Insert & Verify Filtered List Count
		dataValue = "Model Trains";
		item = tree.insert( nullptr, dataValue, InsertAt::FRONT );
		CHECK( filteredTestTree_.size( nullptr ) == 2 );

		// Verify the Item by Value & Index
		result = verifyTreeItemMatch( item, "Model Trains", true );
		CHECK( result == true );
	}

	// Insert an item that should be filtered out
	{
		// Make sure the item inserted isn't in the filtered tree
		size_t oldSize = filteredTestTree_.size( nullptr );
		dataValue = "Worlds";
		item = tree.insert( nullptr, dataValue, InsertAt::BACK );
		CHECK( oldSize == filteredTestTree_.size( nullptr ) );

		// Verify the item inserted still exists in the source tree
		auto found = tree.item( tree.size( nullptr ) - 1, nullptr );
		CHECK( found != nullptr );
		result = verifyTreeItemMatch( found, "Worlds", true );
		CHECK( result == true );
	}

	// Insert a sub-item to another sub-item of the tree root (null parent)
	{
		// Grab the fourth sub-item to the root that will be the parent of our new node
		auto parentItem = tree.item( 4, nullptr );
		CHECK( parentItem != nullptr );

		// Insert a new child into this sub-item
		dataValue = "world_01";
		item = tree.insert( dynamic_cast< UnitTestTreeItem * >( parentItem ), dataValue, InsertAt::FRONT );
		CHECK( item != nullptr );
		CHECK( tree.size( parentItem ) == 1 );

		// Insert a new child that should qualify for the filter "model" and make sure 
		// "Worlds" is added to the index map.
		dataValue = "model_world";
		item = tree.insert( dynamic_cast< UnitTestTreeItem * >( parentItem ), dataValue, InsertAt::BACK );
		CHECK( item != nullptr );
		size_t size = filteredTestTree_.size( parentItem );
		CHECK( size == 1 );
	}
}

TEST_F( TestFixture, removeFromTreeModel )
{
	initialise( TestStringData::STATE_TREE );
	UnitTestTreeModel & tree = testStringData_.getTreeModel();
	CHECK( tree.size( nullptr ) > 0 );

	filteredTestTree_.setSource( &tree );
	filteredTestTree_.setFilter( &filter_ );

	size_t size;
	size_t oldSize;
	bool result = true;

	filter_.setFilterText( "Objects" );
	filteredTestTree_.refresh( true );

	// Remove an item not included in the filtered contents
	{
		oldSize = filteredTestTree_.size( nullptr );

		// Remove "Animations"
		tree.erase( 0, nullptr );
		CHECK( tree.size( nullptr ) == 2 );
		
		result = verifyTreeItemMatch( tree.item( 0, nullptr ), "Animations", true );
		CHECK( !result );

		size = filteredTestTree_.size( nullptr );
		CHECK( oldSize == size );
	}

	// Remove an item that is included in the filter from the previous section (Objects)
	{
		oldSize = filteredTestTree_.size( nullptr );
		auto testItem = tree.item( 0, nullptr );
		
		tree.erase( 0, nullptr );
		CHECK( tree.size( nullptr ) == 1 );
		
		result = verifyTreeItemMatch( tree.item( 0, nullptr ), "Objects", true );
		CHECK( !result );

		size = filteredTestTree_.size( nullptr );
		CHECK( oldSize > size ); 
	}

	// Remove a sub-item ("model_rat") to another child ("Models/Small")
	{
		auto parentItem = dynamic_cast< UnitTestTreeItem * >( tree.item( tree.size( nullptr ) - 1, nullptr ) );
		auto childItem = dynamic_cast< UnitTestTreeItem * >( tree.item( 1, parentItem ) );
		oldSize = tree.size( childItem );
		
		tree.erase( 2, childItem );

		size = tree.size( childItem );
		CHECK( oldSize > size );
	}
}

TEST_F( TestFixture, changeTreeItem )
{
	initialise( TestStringData::STATE_TREE );
	UnitTestTreeModel & tree = testStringData_.getTreeModel();
	CHECK( tree.size( nullptr ) > 0 );

	filteredTestTree_.setSource( &tree );
	filteredTestTree_.setFilter( &filter_ );

	size_t size;
	size_t oldSize;
	std::string dataValue;
	bool result = true;

	filter_.setFilterText( "anim" );
	filteredTestTree_.refresh( true );

	// In the filtered tree before and after the change
	// Filter: "anim"
	// Change "Animations" into "Animals"
	{
		oldSize = filteredTestTree_.size( nullptr );
		dataValue = "Animals";

		tree.update( 0, nullptr, dataValue );
		
		size = filteredTestTree_.size( nullptr );
		CHECK( oldSize == size );

		result = verifyTreeItemMatch( tree.item( 0, nullptr ), "Animals", true );
		CHECK( result == true );
	}

	// Not in the filtered tree before or after the change
	// Filter: "anim"
	// Change "Objects" to "Items" and make sure it doesn't appear in the filtered tree
	{
		oldSize = filteredTestTree_.size( nullptr );
		dataValue = "Items";

		tree.update( 1, nullptr, dataValue );

		size = filteredTestTree_.size( nullptr );
		CHECK( size == oldSize );

		// Set it back to "Objects" for the last test
		dataValue = "Objects";
		tree.update( 1, nullptr, dataValue );
	}

	// Not in the filtered tree, but is after the change
	// Filter: "anim"
	// Change "Models" to "Anims" and make sure it appears in the filtered tree
	{
		oldSize = filteredTestTree_.size( nullptr );
		dataValue = "Anims";

		tree.update( 2, nullptr, dataValue );

		size = filteredTestTree_.size( nullptr );
		CHECK( size > oldSize );
	}

	// In the filtered tree, but not after the change
	// Filter: "terrain"
	// Change "Items" to "Worlds" and make sure it is not in the filtered tree
	{
		// Update the filter
		filter_.setFilterText( "object" );
		filteredTestTree_.refresh( true );
		oldSize = filteredTestTree_.size( nullptr );

		dataValue = "Worlds";
		tree.update( 1, nullptr, dataValue );
		size = filteredTestTree_.size( nullptr );

		// This should remain the same since there are descendants that meet filtering criteria
		CHECK( size == oldSize );

		// Update children to not match the filter. It should remove them from the index map.
		std::stringstream stream;
		auto parent = tree.item( 1, nullptr );
		for (unsigned int i = 0; i < static_cast< unsigned int >( tree.size( parent ) ); ++i)
		{
			auto child = tree.item( i, parent );
			if (child != nullptr)
			{
				stream.clear();
				stream.str( std::string() );
				stream << "Zone" << i;
				dataValue = stream.str();
				tree.update( i, dynamic_cast< UnitTestTreeItem * >( parent ), dataValue );

				for (unsigned int j = 0; j < static_cast< unsigned int >( tree.size( child ) ); ++j)
				{
					stream.clear();
					stream.str( std::string() );
					stream << "zone" << i << "_data0" << j;
					dataValue = stream.str();
					tree.update( j, dynamic_cast< UnitTestTreeItem * >( child ), dataValue );
				}
			}
		}
		
		// Verify that we still have items left in the root index map
		size = filteredTestTree_.size( nullptr );
		CHECK( size == oldSize );
	}
}
