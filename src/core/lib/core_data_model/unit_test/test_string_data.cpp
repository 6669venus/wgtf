#include "pch.hpp"
#include "test_string_data.hpp"
#include "test_data_model_objects.hpp"

struct TestStringData::Implementation
{
	Implementation( TestStringData & self ) 
		: self_( self )
		, state_( STATE_LIST )
		, position_( 0 )
	{
	}

	void initialise( State state )
	{
		/*
		// Note: If raw data is modified, please take into consideration the filtering tests using
		//       positions with specific data expectations (e.g. "apple" should be in index 0).
		*/

		state_ = state;
		switch( state_ )
		{
		case STATE_LIST:
			{
				rawListData_ = "apple berry custard drama eggs fig grape hat igloo jam kangaroo lemon mango noodles ";
				rawListData_ += "orange pineapple queen rice star tribble upvote vine wine xray yoyo zebra ";
				
				position_ = 0;
				std::string dataString = getNextString();
				while (!dataString.empty())
				{
					std::string itemName = dataString.c_str();
					testList_.push_back( itemName );
					dataString = getNextString();
				}
			}
			break;

		case STATE_TREE:
			{	
				rawTreeData_ = "Animations anim_skeleton anim_mouse anim_rat anim_bat fancy_dance ";
				rawTreeData_ += "Models model_skeleton model_mouse model_rat model_bat model_dance ";
				rawTreeData_ += "Objects objects_01 objects_02 objects_03 objects_04 fancy_object ";
				rawTreeData_ += "Mods gui_mod_health alert_system chat_bar action_bar deadly_boss_mods ";
				rawTreeData_ += "Terrain terrain_01 terrain_02 terrain_03 terrain_04 terrain_05 ";
								
				position_ = 0;
				testTree_.initialise( &self_ );
			}
			break;

		default:
			{			
				BWUnitTest::unitTestInfo( "Data Model Unit Test - No Valid State Provided!\n" );
			}
		}
	}

	std::string getNextString()
	{
		std::string data;
		switch (state_)
		{
		case STATE_LIST:
			data = rawListData_;
			break;
		case STATE_TREE:
			data = rawTreeData_;
			break;
		default:
			return "";
		}

		size_t nextPosition = data.find( ' ', position_ );
		size_t count = nextPosition == std::string::npos ? std::string::npos : nextPosition - position_;
		std::string temp = data.substr( position_, count );
		position_ = nextPosition == std::string::npos ? 0 : nextPosition + 1;
		return temp;
	}
	
	TestStringData & self_;
	State state_;
	VariantList testList_;
	UnitTestTreeModel testTree_;
	std::string rawListData_;
	std::string rawTreeData_;
	size_t position_;
};

TestStringData::TestStringData()
	: impl_( new Implementation( *this ) )
{
}

TestStringData::~TestStringData()
{
	// Erase the list
	for (auto it = impl_->testList_.begin(); it != impl_->testList_.end();)
	{
		it = impl_->testList_.erase( it );
	}
}

void TestStringData::initialise( State state )
{
	impl_->initialise( state );
}

VariantList & TestStringData::getVariantList()
{
	return impl_->testList_;
}

UnitTestTreeModel & TestStringData::getTreeModel()
{
	return impl_->testTree_;
}

std::string TestStringData::getNextString()
{
	return impl_->getNextString();
}