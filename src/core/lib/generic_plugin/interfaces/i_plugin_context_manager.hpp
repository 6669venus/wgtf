#ifndef IPLUGIN_CONTEXT_MANAGER
#define IPLUGIN_CONTEXT_MANAGER

//#include "dependency_system/i_interface.hpp"
#include <string>

class IContextManager;

class IPluginContextManager
{
public:
	typedef std::wstring PluginId;

	virtual ~IPluginContextManager() {}
	virtual IContextManager * createContext(const PluginId & id) = 0;
	virtual IContextManager * getContext(const PluginId & id) const = 0;
	virtual IContextManager * getGlobalContext() const = 0;
	virtual void destroyContext(const PluginId & id) = 0;

	virtual void setExecutablePath(const char* path) = 0;
	virtual const char* getExecutablePath() const = 0;
};

#endif ///IPLUGIN_CONTEXT_MANAGER