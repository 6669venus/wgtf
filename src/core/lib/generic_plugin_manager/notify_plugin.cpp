#include "notify_plugin.hpp"

#define STR( X ) #X	
#define PLUGIN_GET_PROC_ADDRESS( hPlugin, func ) \
	::GetProcAddress( hPlugin, STR( func ) )

NotifyPlugin::NotifyPlugin(GenericPluginManager & pluginManager, GenericPluginLoadState loadState)
	: pluginManager_(pluginManager)
	, loadState_(loadState)
{
}

bool NotifyPlugin::operator()(HMODULE hPlugin)
{
	CallbackFunc pCallback = GetPluginCallbackFunc(hPlugin);
	return pCallback ? pCallback(loadState_) : false;
}


CallbackFunc NotifyPlugin::GetPluginCallbackFunc(HMODULE hPlugin)
{
	return (CallbackFunc)PLUGIN_GET_PROC_ADDRESS(hPlugin, PLG_CALLBACK);
}

NotifyPluginPostLoad::NotifyPluginPostLoad(GenericPluginManager & pluginManager)
	: NotifyPlugin(pluginManager, GenericPluginLoadState::PostLoad)
{
}

NotifyPluginPostLoad::~NotifyPluginPostLoad()
{
	pluginManager_.unloadPlugins(pluginsToUnload_);
}

bool NotifyPluginPostLoad::operator()(HMODULE hPlugin)
{
	bool br = NotifyPlugin::operator()(hPlugin);
	if (!br)
	{
		pluginsToUnload_.push_back(hPlugin);
	}
	return br;
}

#undef PLUGIN_GET_PROC_ADDRESS
#undef STR