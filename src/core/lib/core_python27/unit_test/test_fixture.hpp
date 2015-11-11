#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "reflection_test_module.hpp"

#include "core_generic_plugin_test/test_plugin_loader.hpp"
#include "core_python27/scripting_engine.hpp"

#include <stack>
#include <memory>

class ReflectionTestModule;
class IComponentContext;

class TestFixture : public TestPluginLoader
{
public:
	TestFixture( const char * testName,
		TestResult & result );
	virtual ~TestFixture();

	Python27ScriptingEngine& scriptingEngine();
	IComponentContext& context();

private:
	std::stack<IInterface*> interfaces_;
	std::unique_ptr< ReflectionTestModule > reflectionModule_;
	Python27ScriptingEngine scriptingEngine_;
	IComponentContext& context_;
};

#endif
