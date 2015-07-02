//
// Copyright (C) Wargaming 
//

#include "maya_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "generic_plugin/interfaces/i_application.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "qt_common/i_qt_framework.hpp"

#include "../../generic_app/app/memory_plugin_context_creator.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "generic_plugin_manager/config_plugin_loader.hpp"
#include "ngt_event_loop.hpp"
#include "maya_window.hpp"
#include <QtCore/QCoreApplication>

#include <shlwapi.h>

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MSyntax.h>
#include <maya/MGlobal.h>

const char * NGT_MAYA_COMMAND = "NGTMaya";

NGTMayaPlugin::NGTMayaPlugin()
	: ngtEventLoop_( nullptr )
	, mayaWindow_( nullptr )
{
}

NGTMayaPlugin::~NGTMayaPlugin()
{

}

bool NGTMayaPlugin::getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

void *NGTMayaPlugin::creator()
{
	return new NGTMayaPlugin();
}

MStatus NGTMayaPlugin::doIt(const MArgList& args)
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
		auto& contextManager = pluginManager.getContextManager();
		
		auto globalContext = contextManager.getGlobalContext();
		globalContext->registerInterface(new MemoryPluginContextCreator);

		pluginManager.loadPlugins(plugins);

		auto qtFramework = globalContext->queryInterface< IQtFramework >();
		mayaWindow_ = new MayaWindow( *qtFramework );

		auto uiApp = globalContext->queryInterface< IUIApplication >();
		uiApp->addWindow( *mayaWindow_ );

		ngtEventLoop_ = new NGTEventLoop(
			globalContext->queryInterface< IApplication >() );
		ngtEventLoop_->start();

		QObject::connect( QCoreApplication::instance(),
			SIGNAL( QCoreApplication::aboutToQuit() ),
			ngtEventLoop_,
			SLOT(NGTEventLoop::stop()) );
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
	status = plugin.registerCommand( NGT_MAYA_COMMAND, NGTMayaPlugin::creator);

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
	plugin.deregisterCommand( NGT_MAYA_COMMAND );

	return status;
}
