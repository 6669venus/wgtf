#include "generic_plugin/generic_plugin.hpp"

#include "../interfaces/test_interface.hpp"
#include "ngt_core_common/ngt_windows.hpp"

class TestClassA
	: public Implements< INTERFACE_VERSION( TestInterface, 0, 0 ) > // TestInterface v0.0
{
public:
	TestClassA( int value )
		: value_( value )
	{
	}

	void test()
	{
		char buffer[ 256 ];
		sprintf( buffer, "%s - %d\n", typeid( *this ).name(), value_ );
		::OutputDebugStringA( buffer );
	}

private:
	int value_;
};

class TestClassA_0_1
	: public Implements< INTERFACE_VERSION( TestInterface, 0, 1 ) > // TestInterface v0.1
{
public:
	TestClassA_0_1( int value )
		: value_( value )
	{
	}

	void test()
	{
		char buffer[ 256 ];
		sprintf( buffer, "%s - %d\n", typeid( *this ).name(), value_ );
		::OutputDebugStringA( buffer );
	}


	virtual void test2()
	{
		char buffer[ 256 ];
		sprintf( buffer, "%s::test2() - %d\n", typeid( *this ).name(), value_ );
		::OutputDebugStringA( buffer );
	}

private:
	int value_;
};

class TestClassC
	: public Implements< INTERFACE_VERSION( TestInterface, 1, 0 ) > // TestInterface v1.0
{
public:
	TestClassC()
	{
	}

	void test( int value )
	{
		char buffer[ 256 ];
		sprintf( buffer, "%s - %d\n", typeid( *this ).name(), value );
		::OutputDebugStringA( buffer );
	}
};


//==============================================================================
class TestPlugin3
	: public PluginMain
{
public:
	//==========================================================================
	TestPlugin3( IContextManager & contextManager ){}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		contextManager.registerInterface( new TestClassA( 10 ) );
		contextManager.registerInterface( new TestClassA( 20 ) );
		contextManager.registerInterface( new TestClassA( 30 ) );
		contextManager.registerInterface( new TestClassA_0_1( 40 ) );
		contextManager.registerInterface( new TestClassC() );
		return true;
	}

	//==========================================================================
	void Initialise( IContextManager & contextManager )
	{

	}
};

PLG_CALLBACK_FUNC( TestPlugin3 )

