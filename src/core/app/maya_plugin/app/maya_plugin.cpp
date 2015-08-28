//
// Copyright (C) Wargaming 
//
#pragma warning( push )
#pragma warning( disable: 4244 4100 4238 4239 4263 4245 4201 )

#include "core_ui_framework/i_ui_application.hpp"
#include "../../generic_app/app/memory_plugin_context_creator.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/config_plugin_loader.hpp"
#include "core_common/environment.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "ngt_application_proxy.hpp"
#include <QtCore/QCoreApplication>
#include <QtGui/QDockWidget>
#include <QtGui/QLayout>
#include <QtGui/QMainWindow>
#include "qwinhost.h"

#include <shlwapi.h>

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MSyntax.h>
#include <maya/MGlobal.h>
#include <maya/MQtUtil.h>
#include <maya/MTemplateCommand.h>
#include <assert.h>

char NGT_MAYA_COMMAND_SHOW[] = "NGTShow";
char NGT_MAYA_COMMAND_HIDE[] = "NGTHide";
char NGT_MAYA_COMMAND_START[] = "NGTStart";
char NGT_MAYA_COMMAND_STOP[] = "NGTStop";
const char * NGT_MAYA_PLUGIN_LIST_FILE = "\\plugins\\plugins_maya.txt";

#ifdef _DEBUG
	#error Building NGT Maya plugin in debug mode is not supported.
#else
	const char * NGT_MAYA_PLUGIN_NAME = "maya_plugin.mll";
#endif

static HMODULE hApp = ::GetModuleHandleA( NGT_MAYA_PLUGIN_NAME );
;
static char ngtHome[MAX_PATH];

static NGTApplicationProxy * ngtApp = nullptr;
static GenericPluginManager * pluginManager = nullptr;


bool getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

bool loadNGT()
{
	MString filepath = ngtHome;
	filepath += NGT_MAYA_PLUGIN_LIST_FILE;

	std::vector< std::wstring > plugins;
	if (!getNGTPlugins(plugins, filepath.asWChar()) || plugins.empty())
	{
		return MStatus::kFailure; // failed to find any plugins!
	}

	auto& contextManager = pluginManager->getContextManager();
	contextManager.setExecutablePath( ngtHome );

	auto globalContext = contextManager.getGlobalContext();
	globalContext->registerInterface(new MemoryPluginContextCreator);

	pluginManager->loadPlugins(plugins);

	auto uiApp = globalContext->queryInterface< IUIApplication >();
	if (!uiApp)
	{
		return MStatus::kFailure;
	}

	ngtApp = new NGTApplicationProxy( uiApp );
	ngtApp->start();

	return MStatus::kSuccess;
}

struct NGTShowCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_SHOW, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTShowCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	if (!ngtApp->visible())
	{
		ngtApp->show();
	}

	return MStatus::kSuccess;
}

struct NGTHideCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTHideCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	ngtApp->hide();
	return MStatus::kSuccess;
}

struct NGTStartCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTStartCommand::doIt(const MArgList& args)
{
	if (!ngtApp)
	{
		return MStatus::kFailure;
	}

	if (!ngtApp->started())
	{
		ngtApp->start();
	}
	return MStatus::kSuccess;
}

struct NGTStopCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_STOP, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTStopCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	ngtApp->stop();
	return MStatus::kSuccess;
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
	MStatus status;

	if (!Environment::getValue< MAX_PATH >( "NGT_HOME", ngtHome ))
	{
		GetModuleFileNameA( hApp, ngtHome, MAX_PATH );
		PathRemoveFileSpecA( ngtHome );
		PathAppendA( ngtHome, "\\" );
		Environment::setValue( "NGT_HOME", ngtHome );
	}

	pluginManager = new GenericPluginManager();

	NGTShowCommand::registerCommand( obj );
	NGTHideCommand::registerCommand( obj );
	NGTStartCommand::registerCommand( obj );
	NGTStopCommand::registerCommand( obj );

	return loadNGT() ? MStatus::kSuccess : MStatus::kFailure;
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
	NGTShowCommand::deregisterCommand( obj );
	NGTHideCommand::deregisterCommand( obj );
	NGTStartCommand::deregisterCommand( obj );
	NGTStopCommand::deregisterCommand( obj );

	delete ngtApp;
	delete pluginManager;

	ngtApp = nullptr;
	pluginManager = nullptr;

	return MStatus::kSuccess;
}

#pragma warning( pop )
