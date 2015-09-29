#include "pch.hpp"
#include "test_application.hpp"

#include <chrono>

TestApplication::TestApplication()
	: exiting_( false )
{
	thread_ = std::thread( &TestApplication::startApplication, this );
}

TestApplication::~TestApplication()
{
	quitApplication();
	thread_.join();
}

int TestApplication::startApplication()
{
	while (!exiting_)
	{
		notifyUpdate();
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}

	return 0;
}

void TestApplication::quitApplication()
{
	exiting_ = true;
}