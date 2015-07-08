#include "generic_plugin/generic_plugin.hpp"
#include "custom_model_interface_test.hpp"
#include "variant/variant.hpp"
#include "variant/default_meta_type_manager.hpp"

//==============================================================================
class TestDataModelPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestDataModelPlugin( IContextManager & contextManager ) {}

	//==========================================================================
	void Initialise(IContextManager & contextManager) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		customModelInterfaceTest.initialise( contextManager );
	}

private:
	CustomModelInterfaceTest customModelInterfaceTest;
};

PLG_CALLBACK_FUNC( TestDataModelPlugin )

