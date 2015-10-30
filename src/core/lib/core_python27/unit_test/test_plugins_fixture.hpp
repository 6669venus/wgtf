#ifndef TEST_PLUGINS_FIXTURE_HPP
#define TEST_PLUGINS_FIXTURE_HPP

#include "core_generic_plugin_manager/generic_plugin_manager.hpp"


class TestPluginsFixture
{
public:
	TestPluginsFixture();
	virtual ~TestPluginsFixture();

	GenericPluginManager pluginManager_;
};

#endif
