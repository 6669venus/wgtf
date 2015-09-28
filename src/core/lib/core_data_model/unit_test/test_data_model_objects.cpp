#include "pch.hpp"
#include "test_data_model_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_unit_test/unit_test.hpp"

//------------------------------------------------------------------------------

TestFixture::TestFixture()
{
}

void TestFixture::echoListData()
{
	// Debug output for engineers verifying list contents
	VariantList & list = testStringData_.getVariantList();
	std::string value;
	for (unsigned int i = 0; i < static_cast< unsigned int >( list.size() ); ++i)
	{
		auto variant = list[i];
		if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
		{
			continue;
		}

		value = "";
		if (variant.tryCast( value ))
		{
			BWUnitTest::unitTestInfo( "List Item %s\n", value.c_str() );
		}
	}
}

bool TestFixture::findItemInFilteredList( const char * term, bool exactMatch )
{
	if (term == nullptr)
	{
		return false;
	}
		
	std::string value;
	for (unsigned int i = 0; i < static_cast< unsigned int >( filteredTestList_.size() ); ++i)
	{
		auto item = filteredTestList_.item( i );
		Variant variant = item->getData( 0, ValueRole::roleId_ );
		if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
		{
			return false;
		}

		value = "";
		if (variant.tryCast( value ))
		{
			if ( (exactMatch && value.compare( term ) == 0) || 
				 (!exactMatch && value.find( term ) != std::string::npos) )
			{
				return true;
			}
		}
	}

	return false;
}

bool TestFixture::verifyListItemPosition( unsigned int index, const char * value )
{
	if (value == nullptr)
	{
		return false;
	}

	VariantList & list = testStringData_.getVariantList();
		
	std::string itemValue;
	auto item = list.item( index );
	auto variant = item->getData( 0, ValueRole::roleId_ );
	if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
	{
		return false;
	}

	if (variant.tryCast( itemValue ) && itemValue.find( value ) != std::string::npos)
	{
		return true;
	}

	return false;
}

void TestFixture::insertIntoListAtIndex( unsigned int index, const char * value )
{
	unsigned int tracker = 0;
	VariantList & list = testStringData_.getVariantList();

	for (auto & it = list.begin(); it != list.end(); ++it)
	{
		if (tracker == index)
		{
			list.insert( it, value );
			return;
		}

		++tracker;
	}
}

void TestFixture::removeFromListAtIndex( unsigned int index )
{
	unsigned int tracker = 0;
	VariantList & list = testStringData_.getVariantList();

	for (auto & it = list.begin(); it != list.end(); ++it)
	{
		if (tracker == index)
		{
			list.erase( it );
			return;
		}

		++tracker;
	}
}

void TestFixture::getListItemValueAtIndex( unsigned int index, std::string & value )
{
	VariantList & list = testStringData_.getVariantList();
		
	auto item = list.item( index );
	auto variant = item->getData( 0, ValueRole::roleId_ );
	if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
	{
		value = "";
		return;
	}

	variant.tryCast( value );
}

void TestFixture::updateListItemAtIndex( unsigned int index, const char * value )
{
	VariantList & list = testStringData_.getVariantList();
	
	auto item = list.item( index );
	item->setData( 0, ValueRole::roleId_, value );

	filteredTestList_.refresh(true);
}

//------------------------------------------------------------------------------

TestStringData::TestStringData()
{
	position_ = 0;
	rawData_ = "apple berry custard drama eggs fig grape hat igloo jam kangaroo lemon mango noodles ";
	rawData_ += "orange pineapple queen rice star tribble upvote vine wine xray yoyo zebra ";

	std::string dataString = getNextString();
	while (!dataString.empty())
	{
		std::string itemName = dataString.c_str();
		testList_.push_back( itemName );
		dataString = getNextString();
	}
}

TestStringData::~TestStringData()
{
	for (auto it = testList_.begin(); it != testList_.end();)
	{
		it = testList_.erase( it );
	}
}

std::string TestStringData::getNextString()
{
	size_t nextPosition = rawData_.find( ' ', position_ );
	size_t count = nextPosition == std::string::npos ? std::string::npos : nextPosition - position_;
	std::string temp = rawData_.substr( position_, count );
	position_ = nextPosition == std::string::npos ? 0 : nextPosition + 1;
	return temp;
}
