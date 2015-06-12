#include "config_plugin_loader.hpp"

#include <shlwapi.h>

#include <fstream>
#include <iterator>

namespace ConfigPluginLoader
{
typedef std::istream_iterator< std::wstring, wchar_t, std::char_traits< wchar_t > >
	wistream_iterator;

bool getPlugins( std::vector< std::wstring >& plugins, 
				 const std::wstring& configFileName )
{
	std::wifstream inputFile( configFileName.c_str() );
	if (inputFile.good())
	{
		plugins.swap( std::vector< std::wstring > ( 
			wistream_iterator( inputFile ), wistream_iterator() ) );
		return true;
	}

	return false;
}

}
