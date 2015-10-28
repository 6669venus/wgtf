#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "core_python_script/i_scripting_engine.hpp"

#include <memory>

class IComponentContext;
class IObjectManager;
class IDefinitionManager;

class TestFixture
{
public:
	TestFixture();
	virtual ~TestFixture();
	IComponentContext& componentContext() const;
	IObjectManager& objectManager() const;
	IDefinitionManager& definitionManager() const;
	IPythonScriptingEngine& scriptingEngine() const;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
#endif
