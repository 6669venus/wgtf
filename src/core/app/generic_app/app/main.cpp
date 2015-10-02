#include "wg_memory/memory_overrides.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/folder_plugin_loader.hpp"
#include "core_generic_plugin_manager/config_plugin_loader.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "memory_plugin_context_creator.hpp"

#include "core_common/platform_path.hpp"
#include <locale>
#include <codecvt>

namespace
{
	
#ifdef _WIN32
	static const wchar_t* const pluginsFolder = L"plugins\\";
#elif __APPLE__
	static const wchar_t* const pluginsFolder = L"../Resources/plugins/";
#endif // __APPLE__

bool getPlugins (std::vector< std::wstring >& plugins, const wchar_t* configFile)
{
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW( NULL, path, MAX_PATH );
	::PathRemoveFileSpecW( path );

	if (configFile != NULL)
	{
		::PathAppendW( path, pluginsFolder );
		::PathAppendW( path, configFile );
		return ConfigPluginLoader::getPlugins( plugins, path );
	}
	else
	{
		::PathAppendW( path, pluginsFolder );

		return
			ConfigPluginLoader::getPlugins(
				plugins, std::wstring( path ) + L"plugins.txt" ) ||
			FolderPluginLoader::getPluginsCustomPath( plugins, path );
	}
}

}

#ifdef _WIN32
int STDMETHODCALLTYPE WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			 LPSTR lpCmdLine, int nShowCmd )
{
	int argumentCount = 0;
	LPWSTR * arguments = ::CommandLineToArgvW( ::GetCommandLineW(), &argumentCount );
	std::wstring configFile;
	
	for (int i = 0; i < argumentCount - 1; ++i)
	{
		if (::wcscmp( arguments[i], L"--config" ) == 0)
		{
			configFile = arguments[i + 1];
			break;
		}
	}

#endif // _WIN32
	
#ifdef __APPLE__
int main(int argc, char **argv, char **envp, char **apple)
{
	std::wstring configFile;
	for (int i = 0; i < argc - 1; ++i)
	{
		if (::strcmp( argv[i], "--config" ) == 0)
		{
			std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
			configFile = conv.from_bytes(argv[i + 1]).c_str();
			break;
		}
	}
	
#endif // __APPLE__

	std::vector< std::wstring > plugins;
	if (!getPlugins( plugins, configFile.c_str() ) || plugins.empty())
	{
		return 2; // failed to find any plugins!
	}

	int result = 1;

	{
		GenericPluginManager pluginManager;
		IPluginContextManager & contextManager =
			pluginManager.getContextManager();
		contextManager.getGlobalContext()->registerInterface(
			new MemoryPluginContextCreator );

		pluginManager.loadPlugins( plugins );

		IApplication* application =
			contextManager.getGlobalContext()->queryInterface< IApplication >();
		if (application != NULL)
		{
			result = application->startApplication();
		}
	}
	return result;
}
