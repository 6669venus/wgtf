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

	// One item should be in the list
	filter_.setFilterText( "apple" );
	filteredTestList_.refresh( true );
	result = findItemInFilteredList( "pineapple" );
	CHECK( result == true );
	result = findItemInFilteredList( "orange" );
	CHECK( !result );
		
	// Two items should be in the list
	filter_.setFilterText( "orange" );
	filteredTestList_.refresh( true );
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
	CHECK( tree.size( 0 ) > 0 );

	filteredTestTree_.setSource( &tree );
	filteredTestTree_.setFilter( &filter_ );

	unsigned int size;
	bool result = true;
	ITreeModel * sourceTree = filteredTestTree_.getSource();

	// One item should be in the root node with corresponding children
	{
		filter_.setFilterText( "anim" );
		filteredTestTree_.refresh( true );

		// Only one item should remain
		size = static_cast< unsigned int >( filteredTestTree_.size( nullptr ) );
		CHECK( size == 1 );

		// This item should be "Animations"
		auto remainingItem = sourceTree->item( 0, nullptr );
		CHECK( remainingItem != nullptr );
		result = verifyTreeItemMatch( remainingItem, "Animations", true );
		CHECK( result == true );

		// It should have 5 children. "fancy_dance" should remain with its parent.
		size = static_cast< unsigned int >( filteredTestTree_.size( remainingItem ) );
		CHECK( filteredTestTree_.size( remainingItem ) == 5 );
	}

	// No items should be in the tree after it has been filtered with a non-matching term
	{
		filter_.setFilterText( "world" );
		filteredTestTree_.refresh( true );

		// This should now be an empty tree
		size = static_cast< unsigned int >( filteredTestTree_.size( nullptr ) );
		CHECK( size == 0 );
	}

	// Multiple items in multiple root nodes should remain
	{
		filter_.setFilterText( "mod" );
		filteredTestTree_.refresh( true );

		// This should have at least two root items
		size = static_cast< unsigned int >( filteredTestTree_.size( nullptr ) );
		CHECK( size == 2 );
		if (size != 2)
		{
			FAIL( "Incorrect number of items found after filtering on 'mod'. Expected: 2" );
		}

		// The items should be "Models" and "Mods"
		auto firstItem = filteredTestTree_.item( 0, nullptr );
		CHECK( firstItem != nullptr );
		result = verifyTreeItemMatch( firstItem, "Models", true );
		CHECK( result == true );

		auto secondItem = filteredTestTree_.item( 1, nullptr );
		CHECK( secondItem != nullptr );
		result = verifyTreeItemMatch( secondItem, "Mods", true );
		CHECK( result == true );
	}

	// Children of items should remain alongside their parents, but none of the other children will be present
	{
		filter_.setFilterText( "fancy" );
		filteredTestTree_.refresh( true );

		// This should still have two root items
		size = static_cast< unsigned int >( filteredTestTree_.size( nullptr ) );
		CHECK( size == 2 );
		if (size != 2)
		{
			FAIL( "Incorrect number of items found after filtering on 'fancy'. Expected: 2" );
		}

		// The items should only have 1 child each since we filtered on child values, not the parents
		auto firstItem = filteredTestTree_.item( 0, nullptr );
		CHECK( firstItem != nullptr );
		size = static_cast< unsigned int >( filteredTestTree_.size( firstItem ) );
		CHECK( size == 1 );

		auto secondItem = filteredTestTree_.item( 1, nullptr );
		CHECK( secondItem != nullptr );
		size = static_cast< unsigned int >( filteredTestTree_.size( secondItem ) );
		CHECK( size == 1 );
	}
}
