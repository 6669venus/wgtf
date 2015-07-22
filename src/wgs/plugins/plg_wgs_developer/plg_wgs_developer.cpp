//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plugin_main.cpp
//
// main module for plg_wgs_core NGT plugin
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "dependency_system/i_interface.hpp"

#include "wg_types/string_ref.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

#include "qt_common/i_qt_framework.hpp"
#include "qt_common/qt_window.hpp"
#include "qt_common/shared_controls.hpp"

#include <memory>

class WGSDeveloperPlugin
	: public PluginMain
{
public:
	WGSDeveloperPlugin(IContextManager & contextManager)
	{
	}

	bool PostLoad(IContextManager & contextManager) override
	{
		return true;
	}

	void Initialise(IContextManager & contextManager) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();

		Q_INIT_RESOURCE(resources);

		mainWindow_ = uiFramework->createWindow(
			":/resources/main_window.ui", IUIFramework::ResourceType::File);
		uiApplication->addWindow(*mainWindow_);
		mainWindow_->show();

		//createActions(uiFramework);
		//addMenuBar(uiApplication);

		panel_ = uiFramework->createView("qrc:///resources/main_developer.qml", IUIFramework::ResourceType::Url);
		//curvePanel_ = uiFramework.createView("qrc:///plg_wgs_particle_editor/curve_editor", IUIFramework::ResourceType::Url);
		//propertiesPanel_ = uiFramework.createView("qrc:///plg_wgs_particle_editor/properties_editor", IUIFramework::ResourceType::Url);

		//auto model = std::unique_ptr< ITreeModel >(new TestTreeModel());
		//treePanel_ = uiFramework.createView("qrc:///plg_wgs_particle_editor/tree_editor", IUIFramework::ResourceType::Url, std::move(model));

		uiApplication->addView(*panel_);
		//uiApplication.addView(*curvePanel_);
		//uiApplication.addView(*propertiesPanel_);
		//uiApplication.addView(*treePanel_);
	}

	bool Finalise(IContextManager & contextManager) override
	{
		//destroyViews();
		//destroyActions();

		mainWindow_.reset();
		panel_ = nullptr;

		Q_CLEANUP_RESOURCE(resources);

		return true;
	}

	void Unload(IContextManager & contextManager) override
	{
	}

private:
	
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IView > panel_;
};

PLG_CALLBACK_FUNC(WGSDeveloperPlugin)
