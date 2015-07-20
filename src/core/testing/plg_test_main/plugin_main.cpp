#include "generic_plugin/interfaces/i_application.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "../interfaces/test_interface.hpp"
#include "../interfaces/auto_populate.hpp"
#include "dependency_system/i_interface.hpp"

class MainApplication
	: public Implements< IApplication >
{
public:
	void processEvents()
	{
	}

	int startApplication()
	{
		// Query all versions of interface with major version 0
		{
			std::vector< INTERFACE_VERSION( TestInterface, 0, 0 ) * > interfaces;
			Context::queryInterface( interfaces );
			for( auto & pInterface : interfaces )
			{
				pInterface->test();
			}
		}

		// Query only interfaces supporting version 0.1 or greater
		{
			std::vector< INTERFACE_VERSION( TestInterface, 0, 1 ) * > interfaces;
			Context::queryInterface( interfaces );
			for( auto & pInterface : interfaces )
			{
				pInterface->test();
				pInterface->test2();
			}
		}

		// Query only interfaces supporting version 1.0 or greater
		{
			std::vector< INTERFACE_VERSION( TestInterface, 1, 0 ) * > interfaces;
			Context::queryInterface( interfaces );
			int value = 100;
			for( auto & pInterface : interfaces )
			{
				pInterface->test( value );
				// test2() removed
				++value;
			}
		}

		//Test auto populated class
		auto autoPopulate = Context::queryInterface< AutoPopulate >();
		auto pInterface = autoPopulate->getInterfaceA();
		pInterface->test2();

		auto pInterfaces = autoPopulate->getInterfaceBs();
		for ( auto & pInt : pInterfaces )
		{
			pInt->test();
		}
		return 0;
	}

	void registerListener( IApplicationListener * listener ) {};
	void deregisterListener( IApplicationListener * listener ) {};
};

//==============================================================================
class MainTestPlugin
	: public PluginMain
{
public:
	//==========================================================================
	MainTestPlugin( IContextManager & contextManager ) {}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		contextManager.registerInterface( new MainApplication );
		return true;
	}
};

PLG_CALLBACK_FUNC( MainTestPlugin )

