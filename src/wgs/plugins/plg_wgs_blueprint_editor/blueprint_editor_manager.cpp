#include "blueprint_editor_manager.hpp"

#include "command_system/i_command_manager.hpp"
#include "command_system/compound_command.hpp"

#include "interfaces/i_datasource.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"
#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

BlueprintEditorManager::BlueprintEditorManager()
{
}

BlueprintEditorManager::~BlueprintEditorManager()
{
}

void BlueprintEditorManager::init(IUIApplication &uiApplication, IUIFramework &uiFramework)
{
	createActions(uiFramework);
	createViews(uiFramework);

	addActions(uiApplication);
	addViews(uiApplication);
}

void BlueprintEditorManager::fini()
{
	destroyViews();
	destroyActions();
}

void BlueprintEditorManager::createActions(IUIFramework &uiFramework)
{
	// hook new/load/save
	newAction_ = uiFramework.createAction("New", std::bind(&BlueprintEditorManager::newBlueprint, this));
	loadAction_ = uiFramework.createAction("Load", std::bind(&BlueprintEditorManager::loadBlueprint, this));
	saveAction_ = uiFramework.createAction("Save", std::bind(&BlueprintEditorManager::saveBlueprint, this));

	// hook undo/redo
	undoAction_ = uiFramework.createAction("Undo", std::bind(&BlueprintEditorManager::undo, this), std::bind(&BlueprintEditorManager::canUndo, this));
	redoAction_ = uiFramework.createAction("Redo", std::bind(&BlueprintEditorManager::redo, this), std::bind(&BlueprintEditorManager::canRedo, this));
}

void BlueprintEditorManager::createViews(IUIFramework &uiFramework)
{
	auto dataSrc = Context::queryInterface<IDataSource>();
	auto controller = Context::queryInterface<IReflectionController>();

	auto model = std::unique_ptr<ITreeModel>(new ReflectedTreeModel(dataSrc->getBlueprintPage(), controller));
	blueprintView_ = uiFramework.createView("qrc:///wgs/blueprint_panel.qml", IUIFramework::ResourceType::Url, std::move(model));
}

void BlueprintEditorManager::destroyActions()
{
	newAction_.reset();
	loadAction_.reset();
	saveAction_.reset();

	redoAction_.reset();
	undoAction_.reset();
}

void BlueprintEditorManager::destroyViews()
{
	blueprintView_.reset();
}

void BlueprintEditorManager::addActions(IUIApplication & uiApplication)
{
	uiApplication.addAction(*newAction_);
	uiApplication.addAction(*loadAction_);
	uiApplication.addAction(*saveAction_);
	uiApplication.addAction(*undoAction_);
	uiApplication.addAction(*redoAction_);
}

void BlueprintEditorManager::addViews(IUIApplication & uiApplication)
{
	uiApplication.addView(*blueprintView_);
}

void BlueprintEditorManager::undo()
{
	ICommandManager *commandSystemProvider = Context::queryInterface<ICommandManager>();
	assert(commandSystemProvider);
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->undo();
}

void BlueprintEditorManager::redo()
{
	ICommandManager *commandSystemProvider = Context::queryInterface<ICommandManager>();
	assert(commandSystemProvider);
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->redo();
}

bool BlueprintEditorManager::canUndo() const
{
	ICommandManager *commandSystemProvider = Context::queryInterface<ICommandManager>();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canUndo();
}

bool BlueprintEditorManager::canRedo() const
{
	ICommandManager *commandSystemProvider = Context::queryInterface<ICommandManager>();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canRedo();
}

void BlueprintEditorManager::newBlueprint()
{
}

void BlueprintEditorManager::loadBlueprint()
{
}

void BlueprintEditorManager::saveBlueprint()
{
}

bool BlueprintEditorManager::canSaveBlueprint() const
{
	return false;
}