//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plg_file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"

#include "serialization/file_system.hpp"

/**
 * FileSystemPlugin
 *
 * A plugin used to enumerate and manipulate the underlying file system
 */
class FileSystemPlugin
	: public PluginMain
{
public:
	FileSystemPlugin( IContextManager & contextManager )
	{
		
	}

	bool PostLoad( IContextManager & contextManager ) override
	{		
		types_.push_back( contextManager.registerInterface( &fileSystem_, false ) );
		return true;
	}

	void Initialise(IContextManager & contextManager) override
	{
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

private:
	FileSystem fileSystem_;
	std::vector<IInterface*> types_;
};

PLG_CALLBACK_FUNC( FileSystemPlugin )
