#include "plugin_context_manager.hpp"
#include "default_context_manager.hpp"

#include <cassert>

PluginContextManager::PluginContextManager()
	: globalContext_(new DefaultContextManager())
	, executablepath_(nullptr)
{
	globalContext_->registerListener(*this);
}

PluginContextManager::~PluginContextManager()
{
	for (auto & it = contexts_.begin(); it != contexts_.end(); ++it)
	{
		delete it->second;
	}
	globalContext_->deregisterListener(*this);
}

IContextManager* PluginContextManager::createContext(const PluginId & id)
{
	// Create context
	auto pluginContext = new DefaultContextManager(globalContext_.get());

	// Insert in context list
	contexts_.insert(std::make_pair(id, pluginContext));

	// Create ContextCreators and register them
	for (auto & it = contextCreators_.begin(); it != contextCreators_.end(); ++it)
	{
		auto contextCreator = it->second;

		// Create context
		IInterface * pInterface = contextCreator->createContext(id.c_str());
		const char * type = contextCreator->getType();

		// Register
		pluginContext->registerInterfaceImpl(type,
			pInterface,
			IContextManager::Reg_Local);
		childContexts_[contextCreator].push_back(pInterface);
	}
	return pluginContext;
}

IContextManager * PluginContextManager::getContext(const PluginId & id) const
{
	auto findIt = contexts_.find(id);
	if (findIt != contexts_.end())
	{
		return findIt->second;
	}
	return NULL;
}


IContextManager * PluginContextManager::getGlobalContext() const
{
	return globalContext_.get();
}

void PluginContextManager::destroyContext(const PluginId & id)
{
	auto findIt = contexts_.find(id);
	if (findIt != contexts_.end())
	{
		delete findIt->second;
		contexts_.erase(findIt);
	}
}

void PluginContextManager::onPluginContextRegistered(IPluginContextCreator * contextCreator)
{
	// Add ContextCreator to list
	assert(contextCreators_.find(contextCreator->getType()) ==
		contextCreators_.end());
	contextCreators_.insert(
		std::make_pair(contextCreator->getType(), contextCreator));

	// Register interface for ContextCreator
	for (auto context : contexts_)
	{
		IInterface * child =
			context.second->registerInterfaceImpl(
			contextCreator->getType(),
			contextCreator->createContext(context.first.c_str()),
			IContextManager::Reg_Local);
		childContexts_[contextCreator].push_back(child);
	}
}

void PluginContextManager::onPluginContextDeregistered(IPluginContextCreator * contextCreator)
{
	// Remove ContextCreator from list
	for (auto it = contextCreators_.begin();
		it != contextCreators_.end();
		++it)
	{
		if (contextCreator != it->second)
		{
			continue;
		}
		auto findIt = childContexts_.find(contextCreator);
		assert(findIt != childContexts_.end());
		for (auto & child : findIt->second)
		{
			for (auto & contextIt = contexts_.begin(); contextIt != contexts_.end(); ++contextIt)
			{
				if (contextIt->second->deregisterInterface(child))
				{
					break;
				}
			}
		}
		childContexts_.erase(findIt);
		contextCreators_.erase(it);
		return;
	}
}

void PluginContextManager::setExecutablePath(const char* path)
{
	executablepath_ = path;
}

const char* PluginContextManager::getExecutablePath() const
{
	return executablepath_;
}