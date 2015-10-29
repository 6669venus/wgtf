#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_generic_plugin_manager/default_context_manager.hpp"
#include "core_python27/scripting_engine.hpp"

#include <stack>
#include <memory>


class TestFixture
{
public:
	TestFixture();
	virtual ~TestFixture();
	IComponentContext& componentContext();
	IObjectManager& objectManager();
	IDefinitionManager& definitionManager();
	IPythonScriptingEngine& scriptingEngine();

private:
	DefaultComponentContext context_;
	ObjectManager objectManager_;
	DefinitionManager definitionManager_;
	Python27ScriptingEngine scriptingEngine_;
	std::stack<IInterface*> interfaces_;
};
#endif
