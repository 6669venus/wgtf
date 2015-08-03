#ifndef MEMORY_PLUGIN_CONTEXT_CREATOR_HPP
#define MEMORY_PLUGIN_CONTEXT_CREATOR_HPP

#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_dependency_system/i_interface.hpp"

class MemoryPluginContextCreator
	: public Implements< IComponentContextCreator >
{
public:
	IInterface * createContext( const wchar_t * contextId );
	const char * getType() const;
};

#endif //MEMORY_PLUGIN_CONTEXT_CREATOR_HPP



