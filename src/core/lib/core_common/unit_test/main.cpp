#include <stdlib.h>
#include "core_unit_test/unit_test.hpp"


int main( int argc, char* argv[] )
{
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	return BWUnitTest::runTest( "core_common_unit_test", argc, argv );
}

// main.cpp
