#include "generic_plugin/generic_plugin.hpp"

#include "../interfaces/test_interface.hpp"
#include "../interfaces/auto_populate.hpp"
#include "dependency_system/di_ref.hpp"

class TestClassF
	: public Implements< AutoPopulate >
{
public:
	TestClassF( IContextManager & contextManager )
		: testClassA_( contextManager )
		, testClassB_( contextManager )
	{
	}

	InterfaceA * getInterfaceA() { return testClassA_.get(); }
	std::vector< InterfaceB * > getInterfaceBs()
	{
		std::vector< InterfaceB * > returnValue;
		testClassB_.get( returnValue );
		return returnValue;
	}

private:
	//Auto populated references
	DIRef< InterfaceA > testClassA_;
	DIRef< InterfaceB > testClassB_;
};

//==============================================================================
class TestPlugin4
	: public PluginMain
{
public:
	//==========================================================================
	TestPlugin4( IContextManager & contextManager ) {}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		contextManager.registerInterface( new TestClassF( contextManager ) );
		return true;
	}
};

PLG_CALLBACK_FUNC( TestPlugin4 )

