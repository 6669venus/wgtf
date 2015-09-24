#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "scripting_engine.hpp"


/**
 *	Controls initialization and finalization ong Python and
 *	registers the Python interface to be used by other plugins.
 */
class Python27Plugin
	: public PluginMain
{
public:
	Python27Plugin( IComponentContext & contextManager )
		: pInterface_( nullptr )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		const bool transferOwnership = false;
		pInterface_ = contextManager.registerInterface(
			&interpreter_, transferOwnership );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		interpreter_.init();
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		interpreter_.fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
		contextManager.deregisterInterface( pInterface_ );
	}

private:
	IInterface* pInterface_;
	Python27ScriptingEngine interpreter_;
};

PLG_CALLBACK_FUNC( Python27Plugin )

