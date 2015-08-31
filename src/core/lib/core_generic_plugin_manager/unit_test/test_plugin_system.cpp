#include "pch.hpp"



#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/unit_test/plugin1_test/plugin_objects.hpp"
#include "core_generic_plugin_manager/unit_test/plugin1_test/metadata/plugin_objects.mpp"
#include "core_generic_plugin_manager/unit_test/plugin2_test/plugin_objects.hpp"
#include "core_generic_plugin_manager/unit_test/plugin2_test/metadata/plugin_objects.mpp"
#include "core_generic_plugin_manager/unit_test/memory_plugin_context_creator.hpp"


namespace
{
	const wchar_t * s_Plugin1Path = L"plugins/plg_plugin1_test";
	const wchar_t * s_Plugin2Path = L"plugins/plg_plugin2_test";
}
//------------------------------------------------------------------------------
class TestPluginsFixture
{
public:
	TestPluginsFixture()
	{
		IPluginContextManager & contextManager =
			pluginManager_.getContextManager();
//		types_.push_back(
			contextManager.getGlobalContext()->registerInterface(
				new MemoryPluginContextCreator )/* )*/;

		std::vector< std::wstring > plugins;
		plugins.push_back( L"plugins/plg_reflection" );
		pluginManager_.loadPlugins( plugins );
	}
	~TestPluginsFixture()
	{
		std::vector< std::wstring > plugins;
		plugins.push_back( L"plugins/plg_reflection" );
		pluginManager_.unloadPlugins( plugins );

		IPluginContextManager & contextManager =
			pluginManager_.getContextManager();
/*		for( auto type : types_ )
		{
			contextManager.getGlobalContext()->deregisterInterface( type );
		}*/
	}

	GenericPluginManager pluginManager_;
//	std::vector< IInterface * > types_;
};


//------------------------------------------------------------------------------
TEST_F( TestPluginsFixture, load_fixture )
{
	IDefinitionManager * pDefinitionManager =
		pluginManager_.getContextManager().getGlobalContext()->
			queryInterface< IDefinitionManager >();
	CHECK( pDefinitionManager != nullptr );
	IObjectManager * pObjectManager =
		pluginManager_.getContextManager().getGlobalContext()->
			queryInterface< IObjectManager >();
	CHECK( pObjectManager != nullptr );
}


//------------------------------------------------------------------------------
TEST_F( TestPluginsFixture, load_plugin )
{
	auto plugin1 = 
		pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( plugin1 == nullptr );
	
	std::vector< std::wstring > plugins;
	plugins.push_back( s_Plugin1Path );

	pluginManager_.loadPlugins( plugins );

	plugin1 = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( plugin1 != nullptr );

	TestPlugin1TestObjectPtr testObj = nullptr;
	if (plugin1 != nullptr)
	{
		testObj = plugin1->getObject();
		CHECK( testObj != nullptr );

		if (testObj != nullptr)
		{
			std::string text("Sample");
			testObj->setText( text );
			CHECK_EQUAL( text, testObj->getText() );
		}
	}
		
	pluginManager_.unloadPlugins( plugins );
	CHECK( testObj == nullptr );

	plugin1 = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( plugin1 == nullptr );
}


//------------------------------------------------------------------------------
TEST_F( TestPluginsFixture, reload_plugin )
{
	std::vector< std::wstring > plugins;
	plugins.push_back( s_Plugin1Path );

	pluginManager_.loadPlugins( plugins );

	auto rawPointer =
		pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer != nullptr );
	if (rawPointer == nullptr)
	{
		return;
	}
	auto plugin1 = TestPlugin1InterfacePtr::cast( rawPointer->getThis() );
	CHECK( plugin1 != nullptr );

	plugins.clear();
	plugins.push_back( s_Plugin1Path );
	pluginManager_.unloadPlugins( plugins );
	CHECK( plugin1 == nullptr );

	rawPointer = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer == nullptr );
	CHECK( plugin1 == nullptr );

	pluginManager_.loadPlugins( plugins );
	CHECK( plugin1 == nullptr );

	rawPointer = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer != nullptr );
	if (rawPointer == nullptr)
	{
		return;
	}
	plugin1 = TestPlugin1InterfacePtr::cast( rawPointer->getThis() );
	CHECK( plugin1 != nullptr );

	pluginManager_.unloadPlugins( plugins );
	CHECK( plugin1 == nullptr );

	rawPointer = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer == nullptr );
	CHECK( plugin1 == nullptr );
}


////------------------------------------------------------------------------------
TEST_F( TestPluginsFixture, serialise_plugin )
{
	std::vector< std::wstring > plugins;
	plugins.push_back( s_Plugin1Path );

	pluginManager_.loadPlugins( plugins );

	auto rawPointer = 
		pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer != nullptr );
	if (rawPointer == nullptr)
	{
		return;
	}
	auto plugin1 = TestPlugin1InterfacePtr::cast( rawPointer->getThis() );
	CHECK( plugin1 != nullptr );

	TestPlugin1TestObjectPtr testObj = plugin1->getObject();
	CHECK( testObj != nullptr );

	if (testObj != nullptr)
	{
		std::string text("Sample");
		testObj->setText( text );
		CHECK_EQUAL( text, testObj->getText() );
	}

	plugins.clear();
	plugins.push_back( s_Plugin1Path );
	pluginManager_.unloadPlugins( plugins );
	CHECK( plugin1 == nullptr );
	CHECK( testObj == nullptr );
}


//------------------------------------------------------------------------------
TEST_F( TestPluginsFixture, more_plugins )
{
	std::vector< std::wstring > plugins;
	plugins.push_back( s_Plugin1Path );
	plugins.push_back( s_Plugin2Path );
	pluginManager_.loadPlugins( plugins );

	auto rawPointer1 = 
		pluginManager_.queryInterface< ITestPlugin1 >();

	CHECK( rawPointer1 != nullptr );
	if (rawPointer1 == nullptr)
	{
		return;
	}
	auto plugin1 = TestPlugin1InterfacePtr::cast( rawPointer1->getThis() );
	CHECK( plugin1 != nullptr );

	auto rawPointer2 = 
		pluginManager_.queryInterface< ITestPlugin2 >();
	CHECK( rawPointer2 != nullptr );
	if (rawPointer2 == nullptr)
	{
		return;
	}
	auto plugin2 = TestPlugin2InterfacePtr::cast( rawPointer2->getThis() );
	CHECK( plugin2 != nullptr );

	TestPlugin2TestObjectPtr testObj = nullptr;
	if (plugin2 != nullptr)
	{
		testObj = plugin2->getObject();
		CHECK( testObj != nullptr );

		if (testObj != nullptr)
		{
			std::string text("from plugin2");
			testObj->setText( text );
			CHECK_EQUAL( text, testObj->getText() );
		}
	}

	TestPlugin2TestObjectPtr testObj2 = nullptr;
	if (plugin1 != nullptr)
	{
		testObj2 = plugin1->getObjectFromPlugin2();
		CHECK( testObj2 != nullptr );
		CHECK( *testObj == *testObj2 );
	}

	plugins.clear();
	plugins.push_back( s_Plugin1Path );
	pluginManager_.unloadPlugins( plugins );
	CHECK( plugin1 == nullptr );
	CHECK( testObj != nullptr );
	CHECK( testObj2 != nullptr );
	CHECK( *testObj == *testObj2 );

	pluginManager_.loadPlugins( plugins );
	rawPointer1 = pluginManager_.queryInterface< ITestPlugin1 >();
	CHECK( rawPointer1 != nullptr );
	if (rawPointer1 == nullptr)
	{
		return;
	}
	plugin1 = TestPlugin1InterfacePtr::cast( rawPointer1->getThis() );
	CHECK( plugin1 != nullptr );

	if (plugin1 != nullptr)
	{
		testObj2 = plugin1->getObjectFromPlugin2();
		CHECK( testObj2 != nullptr );
		CHECK( *testObj == *testObj2 );
	}

	plugins.clear();
	plugins.push_back( s_Plugin2Path );
	plugins.push_back( s_Plugin1Path );
	pluginManager_.unloadPlugins( plugins );
	CHECK( plugin2 == nullptr );
	CHECK( testObj == nullptr );
	CHECK( testObj2 == nullptr );
}
