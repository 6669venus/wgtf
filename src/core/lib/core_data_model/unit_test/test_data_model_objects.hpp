#ifndef TEST_DATA_MODEL_OBJECTS_HPP
#define TEST_DATA_MODEL_OBJECTS_HPP
#include "pch.hpp"

#include "test_data_model_fixture.hpp"
#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/filtering/string_filter.hpp"
#include <memory>
#include <vector>

//------------------------------------------------------------------------------
class TestStringData
{
public:
	TestStringData();
	~TestStringData();
	VariantList & getVariantList() { return testList_; }

private:
	std::string getNextString();

private:
	VariantList testList_;
	std::string rawData_;
	size_t position_;
};

//------------------------------------------------------------------------------
class TestFixture
	: public TestDataModelFixture
{
public:
	TestFixture();

	// List Model Helper Functions
	void echoListData();
	bool findItemInFilteredList( const char * term, bool exactMatch = false );
	bool verifyListItemPosition( unsigned int index, const char * value );
	void insertIntoListAtIndex( unsigned int index, const char * value );
	void removeFromListAtIndex( unsigned int index );
	void getListItemValueAtIndex( unsigned int index, std::string & value );
	void updateListItemAtIndex( unsigned int index, const char * value );

public:

	// Data for the test(s)
	TestStringData testStringData_;

	// List Models
	FilteredListModel filteredTestList_;

	// Filters
	std::vector< std::string > filterTerms_;
	StringFilter filter_;
};
#endif //TEST_DATA_MODEL_OBJECTS_HPP
