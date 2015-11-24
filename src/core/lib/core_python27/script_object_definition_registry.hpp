#pragma once
#include "core_dependency_system/i_interface.hpp"
#include "i_script_object_definition_registry.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <map>
#include <mutex>


class IClassDefinition;
class IComponentContext;
struct ScriptObjectDefinitionDeleter;


class ScriptObjectDefinitionRegistry: public Implements<IScriptObjectDefinitionRegistry>
{
public:
	ScriptObjectDefinitionRegistry( IComponentContext& context )
		: context_( context )
	{}


	virtual std::shared_ptr<IClassDefinition> getDefinition( const PyScript::ScriptObject& object ) override;


private:
	void removeDefinition( const PyScript::ScriptObject& object, IClassDefinition* definition );

	friend struct ScriptObjectDefinitionDeleter;

	std::map<PyScript::ScriptObject, std::weak_ptr<IClassDefinition>> definitions_;
	std::mutex definitionsMutex_;
	IComponentContext& context_;
};
