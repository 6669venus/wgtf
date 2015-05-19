#include <iostream>


namespace BWUnitTest
{

int runTest( const std::string & testName, int argc, char* argv[] );

int unitTestError( const char * _Format, ... );
int unitTestInfo( const char * _Format, ... );

}


// unit_test.hpp
