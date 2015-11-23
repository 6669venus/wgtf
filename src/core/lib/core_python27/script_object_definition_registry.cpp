#include "pch.hpp"
#include "script_object_definition_registry.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "definition_details.hpp"


struct ScriptObjectDefinitionDeleter
{
	ScriptObjectDefinitionDeleter( const PyScript::ScriptObject& object, ScriptObjectDefinitionRegistry& registry )
		: object_( object )
		, registry_( registry )
	{}


	void operator()( IClassDefinition* definition )
	{
		registry_.deregisterObject( object_, definition );
	}


	const PyScript::ScriptObject object_;
	ScriptObjectDefinitionRegistry& registry_;
};


std::shared_ptr<IClassDefinition> ScriptObjectDefinitionRegistry::registerObject( const PyScript::ScriptObject& object )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	auto itr = definitions_.find( object );

	if (itr != definitions_.end())
	{
		return itr->second.lock();
	}
	
	auto definitionManager = context_.queryInterface<IDefinitionManager>();
	assert( definitionManager != nullptr );

	auto definition =
		definitionManager->registerDefinition( new ReflectedPython::DefinitionDetails( context_, object ) );
	assert( definition != nullptr );

	std::shared_ptr<IClassDefinition> pointer( definition, ScriptObjectDefinitionDeleter( object, *this ) );
	definitions_[object] = pointer;
	return pointer;
}


void ScriptObjectDefinitionRegistry::deregisterObject(
	const PyScript::ScriptObject& object, IClassDefinition* definition )
{
	assert( definition != nullptr );
	IDefinitionManager* definitionManager = definition->getDefinitionManager();

	if (definitionManager != nullptr)
	{
		definitionManager->deregisterDefinition( definition );
	}

	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	auto itr = definitions_.find( object );
	assert( itr != definitions_.end() );
	definitions_.erase( itr );
}
