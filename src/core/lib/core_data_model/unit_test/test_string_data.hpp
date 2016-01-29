#ifndef TEST_STRING_DATA_HPP
#define TEST_STRING_DATA_HPP
#include "pch.hpp"

#include "core_data_model/variant_list.hpp"
#include <memory>
#include <vector>

class UnitTestTreeModel;

class TestStringData
{
public:
	enum State
	{
		STATE_LIST,
		STATE_TREE
	};

	TestStringData();
	~TestStringData();
	void initialise( State state );
	VariantList & getVariantList();
	UnitTestTreeModel & getTreeModel();
	std::string getNextString();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //TEST_STRING_DATA_HPP
