#include "generic_plugin.hpp"

//==============================================================================
PluginMain::PluginMain()
	: name_( NULL )
{
}


//==============================================================================
void PluginMain::init( const char * name )
{
	name_ = name;
}
