#include "folder_plugin_loader.hpp"

#include <shlwapi.h>

namespace 
{
	bool getPluginsInternal( std::vector< std::wstring >& plugins, 
							  const std::wstring & pluginPath)
	{
		WIN32_FIND_DATA find_data;
		HANDLE hFile = ::FindFirstFileW(
			(pluginPath + L"*.dll").c_str(), &find_data );
		while (hFile != INVALID_HANDLE_VALUE)
		{
			plugins.push_back( pluginPath + find_data.cFileName);

			// go for the next DLL
			if (!::FindNextFile( hFile, &find_data ))
			{
				break;
			}
		}

		return true;
	}
}

namespace FolderPluginLoader
{

bool getPluginsCustomPath( std::vector< std::wstring >& plugins, 
						   const std::wstring& pluginPath )
{
	WCHAR fullPath[MAX_PATH];
	::PathCanonicalizeW( fullPath, pluginPath.c_str() );

	return getPluginsInternal( plugins, fullPath );
}

bool loadPluginsExePath( std::vector< std::wstring >& plugins )
{
	WCHAR exePath[MAX_PATH];
	::GetModuleFileNameW( NULL, exePath, MAX_PATH );
	::PathRemoveFileSpecW( exePath );

	return getPluginsInternal( plugins, exePath );
}

}

