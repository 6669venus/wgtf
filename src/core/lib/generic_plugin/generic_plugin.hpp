#ifndef GENERIC_PLUGIN_HPP
#define GENERIC_PLUGIN_HPP

#include "interfaces/i_context_manager.hpp"
#include <cassert>

#ifdef _DEBUG
	#define PLG_CALLBACK PluginCallback_d
#else
	#define PLG_CALLBACK PluginCallback
#endif

enum GenericPluginLoadState
{
	Create,
	PostLoad,
	Initialise,
	Finalise,
	Unload,
	Destroy,
	PluginDestroyEnd
};

#define PLG_CALLBACK_FUNC( PluginType ) \
	PluginMain * createPlugin( IContextManager & contextManager )\
	{\
		auto pluginMain = new PluginType( contextManager );\
		pluginMain->init( #PluginType );\
		return pluginMain;\
	}

class PluginMain
{
public:
	PluginMain();
	virtual ~PluginMain() {}

	virtual bool PostLoad( IContextManager & /*contextManager*/ ) { return true; }
	virtual void Initialise( IContextManager & /*contextManager*/ ) {}
	virtual bool Finalise( IContextManager & /*contextManager*/ ) { return true; }
	virtual void Unload( IContextManager & /*contextManager*/ ) {}

	void init( const char * name );

private:
	const char * name_;
};

#endif //GENERIC_PLUGIN_HPP