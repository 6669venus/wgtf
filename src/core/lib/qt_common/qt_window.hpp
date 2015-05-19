#ifndef QT_WINDOW_HPP
#define QT_WINDOW_HPP

#include "ui_framework/i_window.hpp"
#include "qt_connection_holder.hpp"

#include <map>
#include <memory>
#include <vector>

struct LayoutHint;
class IQtFramework;
class QAction;
class QDockWidget;
class QIODevice;
class QMainWindow;
class QMenuBar;
class QTabWidget;
class QToolBar;
class QWidget;

class QtWindow : public IWindow
{
public:
	QtWindow( IQtFramework & qtFramework, QIODevice & source );
	virtual ~QtWindow();

	const char * id() override;

	void update() override;
	void close() override;

	void show() override;
	void hide() override;

	void addFrame( IView & frame, LayoutHint & hint ) override;
	void addPanel( IView & panel, LayoutHint & hint ) override;
	void addAction( IAction & action, const char * path, const char * shortcut ) override;

	QMainWindow * window() const;

private:
	IQtFramework & qtFramework_;
	std::string id_;
	std::unique_ptr< QMainWindow > mainWindow_;
	std::map< IAction *, QAction * > actions_;
	std::map< IView *, QDockWidget * > panels_;
	std::map< IView *, QWidget * > frames_;
	QtConnectionHolder connections_;

	std::vector< QMenuBar * > menuBars_;
	std::vector< QToolBar * > toolBars_;
	std::vector< QDockWidget * > panelRegions_;
	std::vector< QTabWidget * > frameRegions_;
};

#endif//QT_WINDOW_HPP