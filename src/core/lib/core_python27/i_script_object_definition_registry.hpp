#pragma once
#include "core_dependency_system/i_interface.hpp"

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
	virtual std::shared_ptr<IClassDefinition> registerObject( const PyScript::ScriptObject& object ) = 0;


	/**
	 *	Unregisters a ScriptObject with the engine.
	 */
	virtual void deregisterObject( const PyScript::ScriptObject& object, IClassDefinition* definition ) = 0;
};
