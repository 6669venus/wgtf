#ifndef MAYA_WINDOW_HPP
#define MAYA_WINDOW_HPP

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

class MayaWindow : public IWindow
{
public:
	MayaWindow( IQtFramework & qtFramework );
	virtual ~MayaWindow();

	const char * id() override;

	void update() override;
	void close() override;

	void show() override;
	void hide() override;

	const Menus & menus() const override;
	const Regions & regions() const override;

	QMainWindow * window() const;

private:
	IQtFramework & qtFramework_;
	QMainWindow * mainWindow_;

	std::string id_;
	Menus menus_;
	Regions regions_;
};

#endif//MAYA_WINDOW_HPP