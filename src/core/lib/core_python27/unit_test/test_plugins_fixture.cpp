#include "pch.hpp"
#include "test_plugins_fixture.hpp"

#include "memory_plugin_context_creator.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

TestPluginsFixture::TestPluginsFixture()
{
	IPluginContextManager & contextManager =
		pluginManager_.getContextManager();
		contextManager.getGlobalContext()->registerInterface(
			new MemoryPluginContextCreator );

	std::vector< std::wstring > plugins;
	plugins.push_back( L"plugins/plg_variant" );
	plugins.push_back( L"plugins/plg_reflection" );
	pluginManager_.loadPlugins( plugins );
}
TestPluginsFixture::~TestPluginsFixture()
{
	std::vector< std::wstring > plugins;
	plugins.push_back( L"plugins/plg_variant" );
	plugins.push_back( L"plugins/plg_reflection" );
	pluginManager_.unloadPlugins( plugins );
}
