#include "pch.hpp"
#include <stdlib.h>

int main( int argc, char* argv[] )
{
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	int result = BWUnitTest::runTest(
			"generic_app_plugin_system test", argc, argv );
	return result;
}

// main.cpp
