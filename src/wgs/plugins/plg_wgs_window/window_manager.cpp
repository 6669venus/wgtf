#include "window_manager.hpp"
#include "command_system/compound_command.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"

#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

//==============================================================================
WindowManager::WindowManager()
{
}

//==============================================================================
WindowManager::~WindowManager()
{

}

//==============================================================================
void WindowManager::init(IUIApplication &uiApplication, IUIFramework &uiFramework)
{
	mainWindow_ = uiFramework.createWindow(":/wgs/main_window", IUIFramework::ResourceType::File);
	uiApplication.addWindow(*mainWindow_);
	mainWindow_->show();

	createActions(uiFramework);
	addMenuBar(uiApplication);
}

//------------------------------------------------------------------------------
void WindowManager::fini()
{
	destroyActions();

	mainWindow_.reset();
}

void WindowManager::close()
{
	mainWindow_->close();
}

void WindowManager::createActions(IUIFramework &uiFramework)
{
	// hook application exit
	exitAction_ = uiFramework.createAction("Exit", std::bind(&WindowManager::close, this));
}

void WindowManager::destroyActions()
{
	exitAction_.reset();
}

void WindowManager::addMenuBar(IUIApplication & uiApplication)
{
	uiApplication.addAction(*exitAction_);
}