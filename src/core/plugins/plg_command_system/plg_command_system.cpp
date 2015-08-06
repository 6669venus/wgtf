#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/ui_connection.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/i_object_manager.hpp"

#include "core_command_system/command.hpp"
#include "core_command_system/command_system.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_variant/variant.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include <fstream>
#include <shlwapi.h>
#include <ShellAPI.h>


class CommandSystemPlugin
	: public PluginMain
{
private:
	std::unique_ptr< CommandManager >						commandManager_;

public:
	CommandSystemPlugin( IComponentContext & contextManager )
		: commandManager_( nullptr )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		IDefinitionManager * defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == NULL)
		{
			return false;
		}
		CommandSystem::initReflectedTypes( *defManager );

		commandManager_.reset( new CommandManager( *defManager ) );
		assert( commandManager_ != NULL);
		if (commandManager_ == NULL)
		{
			return false;
		}
		types_.push_back(
			contextManager.registerInterface( commandManager_.get(), false ) );
		commandManager_->init();

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		if(commandManager_ != nullptr)
		{
			commandManager_->fini();
		}
		for ( auto type : types_ )
		{
			 contextManager.deregisterInterface( type );
		}
		commandManager_ = nullptr;
	}

	std::vector< IInterface * > types_;
};


PLG_CALLBACK_FUNC( CommandSystemPlugin )