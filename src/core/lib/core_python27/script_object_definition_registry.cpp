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
		registry_.removeDefinition( object_, definition );
	}


	const PyScript::ScriptObject object_;
	ScriptObjectDefinitionRegistry& registry_;
};


std::shared_ptr<IClassDefinition> ScriptObjectDefinitionRegistry::getDefinition( const PyScript::ScriptObject& object )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	IDefinitionManager* definitionManager = nullptr;
	auto itr = definitions_.find( object );

	if (itr != definitions_.end())
	{
		std::shared_ptr<IClassDefinition> pointer = itr->second.lock();

		if (pointer != nullptr)
		{
			return pointer;
		}

		std::string definitionName = ReflectedPython::DefinitionDetails::generateName( object );
		assert( !definitionName.empty() );

		definitionManager = context_.queryInterface<IDefinitionManager>();
		assert( definitionManager != nullptr );

		auto definition = definitionManager->getDefinition( definitionName.c_str() );
		assert( definition != nullptr );
		definitionManager->deregisterDefinition( definition );
	}
	else
	{
		definitionManager = context_.queryInterface<IDefinitionManager>();
		assert( definitionManager != nullptr );
	}

	auto definition =
		definitionManager->registerDefinition( new ReflectedPython::DefinitionDetails( context_, object ) );
	assert( definition != nullptr );

	std::shared_ptr<IClassDefinition> pointer( definition, ScriptObjectDefinitionDeleter( object, *this ) );
	definitions_[object] = pointer;
	return pointer;
}


void ScriptObjectDefinitionRegistry::removeDefinition(
	const PyScript::ScriptObject& object, const IClassDefinition* definition )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	assert( definition != nullptr );

	auto itr = definitions_.find( object );

	if (itr == definitions_.end() || itr->second.use_count() > 0)
	{
		return;
	}

	definitions_.erase( itr );

	IDefinitionManager* definitionManager = definition->getDefinitionManager();
	assert( definitionManager != nullptr );
	definitionManager->deregisterDefinition( definition );
}
