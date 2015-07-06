//
// Copyright (C) Wargaming 
//

#include "maya_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "generic_plugin/interfaces/i_application.hpp"
#include "ui_framework/i_ui_application.hpp"

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
#ifdef _DEBUG
	const char * NGT_MAYA_PLUGIN_NAME = "ngt_maya_plugin_d.mll";
#else
	const char * NGT_MAYA_PLUGIN_NAME = "ngt_maya_plugin.mll";
#endif

static NGTMayaPlugin _ngt_maya_plugin;

NGTMayaPlugin::NGTMayaPlugin()
	: ngtEventLoop_( nullptr )
	, mayaWindow_( nullptr )
	, ngtLoaded_( false )
	, pluginManager_( new GenericPluginManager )
{
}

NGTMayaPlugin::~NGTMayaPlugin()
{
	delete ngtEventLoop_;
	delete mayaWindow_;
	delete pluginManager_;
}

bool NGTMayaPlugin::getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

bool NGTMayaPlugin::loadNGT( const MArgList& args )
{
	HMODULE hApp = nullptr;

	hApp = ::GetModuleHandle( NGT_MAYA_PLUGIN_NAME );

	char exePath[MAX_PATH];
	GetModuleFileNameA(hApp, exePath, MAX_PATH);
	PathRemoveFileSpecA(exePath);
	PathAppendA(exePath, "\\");
	SetEnvironmentVariableA("Path", exePath);
	
	MString ngtPath = exePath;

	ngtPath += "\\";
	MString filepath = args.asString(0);

	if (filepath.length() == 0)
	{
		filepath = ngtPath + "plugins\\plugins_maya.txt";
	}

	std::vector< std::wstring > plugins;
	if (!getNGTPlugins(plugins, filepath.asWChar()) || plugins.empty())
	{
		return MStatus::kFailure; // failed to find any plugins!
	}

	for (auto& plugin : plugins)
	{
		plugin = std::wstring( ngtPath.asWChar() ) + plugin;
	}

	int result = 1;
	{
		auto& contextManager = pluginManager_->getContextManager();
		
		auto globalContext = contextManager.getGlobalContext();
		globalContext->registerInterface(new MemoryPluginContextCreator);

		pluginManager_->loadPlugins(plugins);

		mayaWindow_ = new MayaWindow();

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

	ngtLoaded_ = true;
	return MStatus::kSuccess;
}

MStatus NGTMayaPlugin::doIt(const MArgList& args)
{
	if (ngtLoaded_)
	{
		return MStatus::kSuccess;
	}

	return loadNGT( args ) ? MStatus::kSuccess : MStatus::kFailure;
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

	_ngt_maya_plugin.doIt( MArgList() );

	// Add plug-in feature registration here
	//	
	status = _ngt_maya_plugin.registerCommand( obj );

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
	MFnPlugin plugin(obj);

	// Add plug-in feature deregistration here
	//
	return _ngt_maya_plugin.deregisterCommand( obj );
}
