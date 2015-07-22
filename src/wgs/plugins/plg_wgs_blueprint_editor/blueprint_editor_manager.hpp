#ifndef BLUPRINT_EDITOR_MANAGER_H
#define BLUPRINT_EDITOR_MANAGER_H

#include <memory>

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;

class BlueprintEditorManager
{    
public:
    explicit BlueprintEditorManager();
    ~BlueprintEditorManager();

	void init(IUIApplication &uiApplication, IUIFramework &uiFramework);
	void fini();

private:
	void onIdle();

	void createActions(IUIFramework &uiFramework);
	void createViews(IUIFramework &uiFramework);

	void destroyActions();
	void destroyViews();

	void addActions(IUIApplication &uiApplication);
	void addViews(IUIApplication &uiApplication);

private:
	std::unique_ptr<IAction> newAction_;
	std::unique_ptr<IAction> loadAction_;
	std::unique_ptr<IAction> saveAction_;
	std::unique_ptr<IAction> undoAction_;
	std::unique_ptr<IAction> redoAction_;

	std::unique_ptr<IView> blueprintView_;

	void newBlueprint();
	void loadBlueprint();
	void saveBlueprint();
	bool canSaveBlueprint() const;

	void close();
	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
};

#endif // BLUPRINT_EDITOR_MANAGER_H
