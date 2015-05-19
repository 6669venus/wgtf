#include "generic_plugin_system/generic_plugin.hpp"
#include "generic_plugin_system/generic_plugin_manager.hpp"
#include "generic_plugin_system/interfaces/ui_connection.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/i_object_manager.hpp"

#include "command_system/reflected_command.hpp"
#include "command_system/command_system.hpp"
#include "command_system/command_manager.hpp"
#include "variant/variant.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "serialization/resizing_memory_stream.hpp"
#include <fstream>
#include <shlwapi.h>
#include <ShellAPI.h>

namespace
{
	class UndoRunnable
		: public Runnable
	{
	public:
		UndoRunnable( CommandSystemProvider & commandSystemProvider )
			: commandSystemProvider_( commandSystemProvider )
		{
		}

		~UndoRunnable()
		{
		}

		void execute() override
		{
			commandSystemProvider_.undo();
		}

		bool isComplete() const override
		{
			return false;
		}

	private:
		CommandSystemProvider & commandSystemProvider_;
	};


	//==========================================================================
	class RedoRunnable
		: public Runnable
	{
	public:
		RedoRunnable( CommandSystemProvider & commandSystemProvider )
			: commandSystemProvider_( commandSystemProvider )
		{
		}

		~RedoRunnable()
		{
		}

		void execute()
		{
			commandSystemProvider_.redo();
		}

		bool isComplete() const
		{
			return false;
		}

	private:
		CommandSystemProvider & commandSystemProvider_;
	};

	
}

class CommandSystemPlugin
	: public PluginMain
{
private:
	std::unique_ptr< CommandManager >						commandManager_;
	std::unique_ptr< UndoRunnable >		undoRunnable_;
	std::unique_ptr< RedoRunnable >		redoRunnable_;
	UIConnection						undoConnection_;
	UIConnection						redoConnection_;

public:
	CommandSystemPlugin( IContextManager & contextManager )
		: commandManager_( nullptr )
	{
	}

	bool PostLoad( IContextManager & contextManager ) override
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

	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		undoRunnable_.reset(
			new UndoRunnable( *commandManager_ ) );
		redoRunnable_.reset(
			new RedoRunnable( *commandManager_ ) );

		/*UIProvider * uiProvider = 
			contextManager.queryInterface< UIProvider >();
		if (uiProvider == nullptr)
		{
			return;
		}

		redoConnection_ = 
			uiProvider->registerShortcut( "Ctrl+Y", *redoRunnable_ );
		undoConnection_ = 
			uiProvider->registerShortcut( "Ctrl+Z", *undoRunnable_ );*/
	}

	bool Finalise( IContextManager & contextManager ) override
	{

		/*auto uiProvider = contextManager.queryInterface< UIProvider >();
		if (uiProvider != nullptr)
		{
			uiProvider->deregisterShortcut( undoConnection_ );
			uiProvider->deregisterShortcut( redoConnection_ );
		}
		undoConnection_ = nullptr;
		redoConnection_ = nullptr;*/

		undoRunnable_ = nullptr;
		redoRunnable_ = nullptr;

		return true;
	}

	void Unload( IContextManager & contextManager ) override
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

