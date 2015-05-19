#include "generic_plugin_system/generic_plugin.hpp"

#include "../interfaces/test_interface.hpp"
#include "../interfaces/auto_populate.hpp"
#include "dependency_system/di_ref.hpp"

class TestClassF
	: public Implements< AutoPopulate >
{
public:
	InterfaceA * getInterfaceA() { return testClassA_.get(); }
	std::vector< InterfaceB * > getInterfaceBs() { return testClassB_.get(); }

private:
	//Auto populated references
	DIRef< InterfaceA > testClassA_;
	DIRefMany< InterfaceB > testClassB_;
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
		contextManager.registerInterface( new TestClassF );
		return true;
	}
};

PLG_CALLBACK_FUNC( TestPlugin4 )

