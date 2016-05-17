#ifndef FOLDER_PLUGIN_LOADER_HPP
#define FOLDER_PLUGIN_LOADER_HPP

#include <string>
#include <vector>

namespace wgt
{
namespace FolderPluginLoader
{
	bool getPluginsCustomPath( std::vector< std::wstring >& plugins, 
							   const std::wstring& pluginPath );
	bool getPluginsExePath();
}

}
#endif // FOLDER_PLUGIN_LOADER_HPP

