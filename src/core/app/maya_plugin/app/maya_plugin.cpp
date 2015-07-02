//
// Copyright (C) Wargaming 
// 

//#define NT_PLUGIN
//#define REQUIRE_IOSTREAM  
#define AW_NEW_IOSTREAMS  

#include "maya_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "generic_plugin/interfaces/i_application.hpp"
#include "../../generic_app/app/memory_plugin_context_creator.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "generic_plugin_manager/config_plugin_loader.hpp"

#include <shlwapi.h>

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MSyntax.h>
#include <maya/MGlobal.h>

MayaGenericApp::MayaGenericApp() 
{

}

MayaGenericApp::~MayaGenericApp() 
{

}

bool MayaGenericApp::getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

void *MayaGenericApp::creator() 
{
	return new MayaGenericApp();
}

MStatus MayaGenericApp::doIt(const MArgList& args) 
{
	MStatus status;
	MString filepath = args.asString(0);

	std::vector< std::wstring > plugins;
	if (!getNGTPlugins(plugins, filepath.asWChar()) || plugins.empty())
	{
		return status; // failed to find any plugins!
	}

	HMODULE hApp = nullptr;

#ifdef _DEBUG
	hApp = ::GetModuleHandle("maya_generic_app_d.mll");
#else
	hApp = ::GetModuleHandle("maya_generic_app.mll");
#endif

	char exePath[MAX_PATH];
	GetModuleFileNameA(hApp, exePath, MAX_PATH);
	PathRemoveFileSpecA(exePath);
	PathAppendA(exePath, "\\");
	SetEnvironmentVariableA("Path", exePath);
	
	int result = 1;
	{
		GenericPluginManager pluginManager;
		IPluginContextManager& contextManager = pluginManager.getContextManager();
		
		contextManager.getGlobalContext()->registerInterface(new MemoryPluginContextCreator);	
		//contextManager.setExecutablePath(exePath);

		pluginManager.loadPlugins(plugins);

		IApplication* application = contextManager.getGlobalContext()->queryInterface< IApplication >();
		if (application != NULL)
		{
			result = application->startApplication();
		}
	}

	return status;
}


PLUGIN_EXPORT MStatus initializePlugin(MObject obj)
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin(obj, "Wargaming", "2015");

	// Add plug-in feature registration here
	//	
	status = plugin.registerCommand("MayaGenericApp", MayaGenericApp::creator);

	return status;
}

PLUGIN_EXPORT MStatus uninitializePlugin(MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin(obj);

	// Add plug-in feature deregistration here
	//
	plugin.deregisterCommand("MayaGenericApp");

	return status;
}
