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
<<<<<<< HEAD
=======
	Python27ScriptingEngine scriptingEngine_;
>>>>>>> e3011fd8b5530effe3ef61fddf24f480bc61166f
	IComponentContext& context_;
};

#endif
