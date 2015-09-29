#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "scenario.hpp"
#include "scripting_engine.hpp"


/**
 *	Controls initialization and finalization ong Python and
 *	registers the Python interface to be used by other plugins.
 */
class Python27Plugin
	: public PluginMain
{
public:
	Python27Plugin( IComponentContext & contextManager )
		: pInterface_( nullptr )
		, pObjectManager_( nullptr )
		, pDefinitionManager_( nullptr )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		pObjectManager_ =
			contextManager.queryInterface< IObjectManager >();
		if (pObjectManager_ == nullptr)
		{
			return false;
		}

		pDefinitionManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager_ == nullptr)
		{
			return false;
		}

		const bool transferOwnership = false;
		pInterface_ = contextManager.registerInterface(
			&interpreter_, transferOwnership );

		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		IDefinitionManager& definitionManager = (*pDefinitionManager_);
		REGISTER_DEFINITION( Scenario );

		IObjectManager& objectManager = (*pObjectManager_);

		interpreter_.init( definitionManager, objectManager );
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		interpreter_.fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
		contextManager.deregisterInterface( pInterface_ );
	}

private:
	IInterface * pInterface_;

	IObjectManager * pObjectManager_;
	IDefinitionManager * pDefinitionManager_;

	Python27ScriptingEngine interpreter_;
};

PLG_CALLBACK_FUNC( Python27Plugin )

