#ifndef TEST_STRUCTURE_HPP
#define TEST_STRUCTURE_HPP

#include "test_macros.hpp"
#include <vector>
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "reflection/reflected_object.hpp"

typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

struct TestStructure
{
	TestStructure();

	void init( const IDefinitionManager & defManager );

	DEFINE_TEST_DATA_TYPES()
};

struct TestInheritedStruct
	: public TestStructure
{
	TestInheritedStruct();

	void init( const IDefinitionManager & defManager );

	DEFINE_INHERITS_TEST_DATA_TYPES()
};

#endif //TEST_STRUCTURE