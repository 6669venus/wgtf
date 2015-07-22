
#include "engine_adapter.h"

#include "game_common.h"
#include "game_loader.h"
#include "game_type_provider.h"
#include "game_filesystem.h"
#include "game_viewport.h"

#include "generic_plugin/interfaces/i_context_manager.hpp"

WGSEngineAdapter::WGSEngineAdapter()
	: gameLoader_(new GameBinaryLoader())
	, typeProvider_(new GameTypeProvider()) 
{
	gameViewport_ = std::make_unique<GameViewport>(*gameLoader_.get());
	fileSystem_ = std::make_unique<GameFileSystem>(*gameLoader_.get());
}

WGSEngineAdapter::~WGSEngineAdapter()
{

}

bool WGSEngineAdapter::initApp(const char* project)
{

	if (!gameLoader_->CreateToolsApp(project))
	{
		return false;
	}
	
	return true;
}

bool WGSEngineAdapter::registerComponentTypes(IContextManager & contextManager)
{
	if (!typeProvider_->Initialize(contextManager, *gameLoader_.get()))
	{
		return false;
	}

	return true;
}



bool WGSEngineAdapter::init(IContextManager & contextManager)
{
	interfaces_.push_back(contextManager.registerInterface(gameViewport_.get(), false));
	interfaces_.push_back(contextManager.registerInterface(fileSystem_.get(), false));
	
	return true;
}

void WGSEngineAdapter::fini(IContextManager & contextManager)
{
	
	typeProvider_->Term(contextManager);
	gameLoader_->Term();
}

void WGSEngineAdapter::postLoad(IContextManager & contextManager)
{
}

void WGSEngineAdapter::unload(IContextManager & contextManager)
{
	for (auto i : interfaces_)
		contextManager.deregisterInterface(i);

	interfaces_.clear(); 
}
