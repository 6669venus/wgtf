#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "interpreter.hpp"

class Python27Plugin
	: public PluginMain
{
public:
	Python27Plugin( IComponentContext & contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Python27Interpreter::init();
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		Python27Interpreter::fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
	}
};

PLG_CALLBACK_FUNC( Python27Plugin )

