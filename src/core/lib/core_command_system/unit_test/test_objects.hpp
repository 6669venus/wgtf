#ifndef TEST_OBJECTS2_HPP
#define TEST_OBJECTS2_HPP
#include "pch.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/binary_block.hpp"
#include <vector>

#include "test_command_system_fixture.hpp"
#include <memory>


//------------------------------------------------------------------------------
class TestCommandObject 
{
	DECLARE_REFLECTED

public:
	void setCounter( const int & value ) { counter_ = value; }
	void getCounter( int * value ) const { *value = counter_; }

	void setText( const std::string & value ) { text_ = value; }
	const std::string & getText() const { return text_; }

	TestCommandObject();
	bool operator==( const TestCommandObject& tdo ) const;
	bool operator!=( const TestCommandObject& tdo ) const;

	void initialise( int value );
	
public:
	int counter_;

	std::string text_;

	// PropertyType::String
	std::string string_;
	std::vector< std::string > strings_;

	// PropertyType::WString
	std::wstring wstring_;
	std::vector< std::wstring > wstrings_;

	// PropertyType::Boolean
	bool boolean_;
	std::vector< bool > booleans_;

	// PropertyType::UInt32
	uint32_t uint32_;
	std::vector< uint32_t > uint32s_;

	// PropertyType::Int32
	int32_t int32_;
	std::vector< int32_t > int32s_;

	// PropertyType::UInt64
	uint64_t uint64_;
	std::vector< uint64_t > uint64s_;

	// PropertyType::Float
	float float_;
	std::vector< float > floats_;

	// PropertyType::Raw_Data,
	std::shared_ptr< BinaryBlock > binary_;
	std::vector< std::shared_ptr< BinaryBlock > > binaries_;
};


//------------------------------------------------------------------------------
class TestCommandFixture
	: public TestCommandSystemFixture
{
public:
	TestCommandFixture()
	{
		IDefinitionManager & definitionManager = getDefinitionManager();
		REGISTER_DEFINITION( TestCommandObject );
		klass_ = definitionManager.getDefinition< TestCommandObject>();
	}

	void fillValuesWithNumbers(Collection& values)
	{
		float increment = 3.25f;
		float value = 1.0f;
		for (int i = 0; i < 5; ++i)
		{
			auto it = values.insert(i);
			it.setValue(value);
			value += increment;
			increment += 3.25f;
		}
	}

public:
	IClassDefinition * klass_;
};
#endif //TEST_OBJECTS2_HPP
