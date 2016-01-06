#ifndef TEST_METHODS_OBJECT_HPP
#define TEST_METHODS_OBJECT_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"

class TestMethodsObject
{
	DECLARE_REFLECTED

public:
	void testMethod1();
	std::string testMethod2();
	std::string testMethod3( int parameter );
	std::string testMethod4( const std::string& parameter1, int parameter2 );
	std::string testMethod5( std::string& parameter );
	void testMethod6( std::string* parameter );
	int testMethod7( int parameter );
	double testMethod8( double parameter );
};

#endif //TEST_METHODS_OBJECT
