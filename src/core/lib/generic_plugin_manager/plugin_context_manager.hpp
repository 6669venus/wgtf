#ifndef PLUGIN_CONTEXT_MANAGER_HPP
#define PLUGIN_CONTEXT_MANAGER_HPP

#include "generic_plugin\interfaces\i_plugin_context_manager.hpp"
#include "dependency_system\i_interface.hpp"
#include "generic_plugin\interfaces\i_plugin_context_creator.hpp"
#include "generic_plugin\interfaces\i_context_manager.hpp"

#include <map>
#include <memory>
#include <string>

class PluginContextManager
	: public Implements < IPluginContextManager >
	, public IContextManagerListener
{
	typedef std::vector< IPluginContextCreator * > ContextCreatorCollection;

public:
	PluginContextManager();
	virtual ~PluginContextManager();

	IContextManager * createContext(const PluginId & id);
	IContextManager * getContext(const PluginId & id) const;
	IContextManager * getGlobalContext() const;
	void destroyContext(const PluginId & id);

	virtual void onPluginContextRegistered(IPluginContextCreator *) override;
	virtual void onPluginContextDeregistered(IPluginContextCreator *) override;

	void setExecutablePath(const char* path);
	const char* getExecutablePath() const;

private:
	typedef std::vector< IInterface * > InterfaceCollection;
	typedef std::map< IPluginContextCreator *, InterfaceCollection > ContextChildrenCollection;
	ContextChildrenCollection							childContexts_;
	std::map< PluginId, IContextManager * >				contexts_;
	std::map< std::string, IPluginContextCreator * >	contextCreators_;
	std::unique_ptr< IContextManager >					globalContext_;
	const char*											executablepath_;
};

#endif //PLUGIN_CONTEXT_MANAGER_HPP
