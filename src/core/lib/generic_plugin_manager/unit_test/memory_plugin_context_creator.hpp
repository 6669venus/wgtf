#ifndef MEMORY_PLUGIN_CONTEXT_CREATOR_HPP
#define MEMORY_PLUGIN_CONTEXT_CREATOR_HPP

#include "generic_plugin/interfaces/i_plugin_context_creator.hpp"
#include "dependency_system/i_interface.hpp"

class MemoryPluginContextCreator
	: public Implements< IPluginContextCreator >
{
public:
	IInterface * createContext( const wchar_t * contextId );
	const char * getType() const;
};

#endif //MEMORY_PLUGIN_CONTEXT_CREATOR_HPP



