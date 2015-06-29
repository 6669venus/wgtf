#ifndef GENERIC_PLUGIN_MANAGER_HPP
#define GENERIC_PLUGIN_MANAGER_HPP

#include "ngt_core_common/ngt_windows.hpp"
#include "generic_plugin/interfaces/i_memory_allocator.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

class IPluginContextManager
{
public:
	typedef std::wstring PluginId;

	virtual ~IPluginContextManager() {}
	virtual IContextManager * createContext( const PluginId & id ) = 0;
	virtual IContextManager * getContext( const PluginId & id ) const = 0;
	virtual IContextManager * getGlobalContext() const = 0;
	virtual void destroyContext( const PluginId & id ) = 0;
};

class GenericPluginManager
{
public:
	typedef std::vector< HMODULE > PluginList;

	GenericPluginManager();
	virtual ~GenericPluginManager();

	void loadPlugins( const std::vector< std::wstring >& plugins );

	void unloadPlugins( const std::vector< std::wstring >& plugins );
	void unloadPlugins( const PluginList& plugins );
	
	IPluginContextManager & getContextManager() const;

	template< class T >
	T * queryInterface()
	{
		return reinterpret_cast< T * >( queryInterface(
			typeid( T ).name() ) );
	}

private:
	void * queryInterface( const char * name ) const;

	typedef std::function< bool (HMODULE) > NotifyFunction;
	void notifyPlugins( const PluginList& plugins, NotifyFunction func );

	HMODULE loadPlugin( const std:: wstring& filename );
	bool unloadPlugin( HMODULE hPlugin );
	void unloadContext( HMODULE hPlugin );

	PluginList plugins_;
	std::map< std::wstring, IMemoryAllocator * >	memoryContext_;
	std::unique_ptr< IPluginContextManager >		contextManager_;
};

#endif //GENERIC_PLUGIN_MANAGER_HPP