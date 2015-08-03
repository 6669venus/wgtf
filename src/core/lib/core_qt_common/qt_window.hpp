#ifndef QT_WINDOW_HPP
#define QT_WINDOW_HPP

#include "core_ui_framework/i_window.hpp"
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

	const char * id() const override;
	const char * title() const override;
	void update() override;
	void close() override;

	void show() override;
	void hide() override;

	const Menus & menus() const override;
	const Regions & regions() const override;

	virtual void * nativeWindowId() const override;
	virtual void makeFramelessWindow() override;

	// TODO: remove the need for this
	QMainWindow * window() const;

private:
	IQtFramework & qtFramework_;
	std::unique_ptr< QMainWindow > mainWindow_;

	std::string id_;
	Menus menus_;
	Regions regions_;
};

#endif//QT_WINDOW_HPP