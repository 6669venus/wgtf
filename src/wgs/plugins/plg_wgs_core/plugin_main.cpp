//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plugin_main.cpp
//
// main module for plg_wgs_core NGT plugin
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_component_context.hpp"
#include "dependency_system/i_interface.hpp"

#include "serialization/interfaces/i_file_utilities.hpp"
#include "wg_types/string_ref.hpp"

#include "engine_adapter.h"

class WGSFileUtilities
	: public Implements < IFileUtilities >
{
	StringRef getFilename(const StringRef & file)
	{
		return StringRef("", 0);
	}


	StringRef getExtension(const StringRef & file)
	{
		return StringRef("", 0);
	}

	StringRef removeExtension(const StringRef & file)
	{
		return StringRef("", 0);
	}

	bool fileExists(const StringRef & file) override
	{
		return false;
	}

	std::string resolveFilename(const StringRef & file) override
	{
		return std::string(file.data(), file.length());
	}
};


/**
* FileSystemPlugin
*
* A plugin used to enumerate and manipulate the underlying file system
*/
class DespairCorePlugin
	: public PluginMain
{
public:

	DespairCorePlugin(IComponentContext & contextManager)
	{
		contextManager.registerInterface(&fileUtils_, false);
		contextManager.registerInterface(&engineAdapter_, false);
	}

	bool PostLoad(IComponentContext & contextManager) override
	{
		engineAdapter_.postLoad(contextManager);
		return true;
	}

	void Initialise(IComponentContext & contextManager) override
	{
		engineAdapter_.init(contextManager);
	}

	bool Finalise(IComponentContext & contextManager) override
	{
		engineAdapter_.fini(contextManager);
		return true;
	}

	void Unload(IComponentContext & contextManager) override
	{
		engineAdapter_.unload(contextManager);
	}

private:

	std::vector< IInterface * > types_;
	WGSFileUtilities	fileUtils_;
	WGSEngineAdapter	engineAdapter_;
};

PLG_CALLBACK_FUNC(DespairCorePlugin)
