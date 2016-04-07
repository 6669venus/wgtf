#include "core_generic_plugin/generic_plugin.hpp"
#include "custom_model_interface_test.hpp"

//==============================================================================
class TestDataModelPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestDataModelPlugin( IComponentContext & contextManager ) {}

	//==========================================================================
	void Initialise(IComponentContext & contextManager) override
	{
		customModelInterfaceTest.initialise( contextManager );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager ) override
	{
		customModelInterfaceTest.fini( contextManager );
		return true;
	}

private:
	CustomModelInterfaceTest customModelInterfaceTest;
};

PLG_CALLBACK_FUNC( TestDataModelPlugin )

