#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "test_plugins_fixture.hpp"
#include "reflection_test_module.hpp"

#include "core_python27/scripting_engine.hpp"

#include <stack>
#include <memory>

class ReflectionTestModule;

class TestPythonFixture : public TestPluginsFixture
{
public:
	TestPythonFixture();
	virtual ~TestPythonFixture();

	Python27ScriptingEngine scriptingEngine_;

private:
	std::stack<IInterface*> interfaces_;
	std::unique_ptr< ReflectionTestModule > reflectionModule_;
};
#endif
