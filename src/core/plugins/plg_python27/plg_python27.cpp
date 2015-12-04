#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_python27/script_object_definition_registry.hpp"
#include "core_python27/type_converters/converter_queue.hpp"


/**
 *	Controls initialization and finalization of Python and
 *	registers the Python interface to be used by other plugins.
 */
class Python27Plugin
	: public PluginMain
{
public:
	Python27Plugin( IComponentContext & contextManager )
		: interpreter_( contextManager )
		, definitionRegistry_( contextManager )
		, typeConverterQueue_( contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		const bool transferOwnership = false;
		interfaces_.push( contextManager.registerInterface( &interpreter_, transferOwnership ) );
		interfaces_.push( contextManager.registerInterface( &definitionRegistry_, transferOwnership ) );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		auto pDefinitionManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager_ == nullptr)
		{
			return;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager_);
		REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
		REGISTER_DEFINITION( Scenario );

		interpreter_.init();
		typeConverterQueue_.init();
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		typeConverterQueue_.fini();
		interpreter_.fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
		while (!interfaces_.empty())
		{
			contextManager.deregisterInterface( interfaces_.top() );
			interfaces_.pop();
		}
	}

private:
	std::stack<IInterface*> interfaces_;
	Python27ScriptingEngine interpreter_;
	ScriptObjectDefinitionRegistry definitionRegistry_;
	PythonType::ConverterQueue typeConverterQueue_;
};

PLG_CALLBACK_FUNC( Python27Plugin )

