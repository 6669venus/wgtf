#pragma once
#include "i_script_object_definition_registry.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <map>
#include <mutex>


class IClassDefinition;
class IComponentContext;


class ScriptObjectDefinitionRegistry: public Implements<IScriptObjectDefinitionRegistry>
{
public:
	ScriptObjectDefinitionRegistry( IComponentContext& context )
		: context_( context )
	{}


	virtual std::shared_ptr<IClassDefinition> registerObject( const PyScript::ScriptObject& object ) override;
	virtual void deregisterObject( const PyScript::ScriptObject& object, IClassDefinition* definition ) override;


private:
	std::map<PyScript::ScriptObject, std::weak_ptr<IClassDefinition>> definitions_;
	std::mutex definitionsMutex_;
	IComponentContext& context_;
};
