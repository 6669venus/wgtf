#pragma once
#include <memory>


class IClassDefinition;

namespace PyScript
{
	class ScriptObject;
}


/**
 *	This class is for keeping a unique class definition for every Python object.
 */
struct IScriptObjectDefinitionRegistry
{
	/**
	 *	Registers a ScriptObject with the engine.
	 *  @return a IClassDefinition for the associated ScriptObject.
	 */
	virtual std::shared_ptr<IClassDefinition> getDefinition( const PyScript::ScriptObject& object ) = 0;
};
