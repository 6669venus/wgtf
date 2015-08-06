#ifndef MAYA_WINDOW_HPP
#define MAYA_WINDOW_HPP

#include "core_ui_framework/i_window.hpp"

#include <string>

class QMainWindow;

class MayaWindow : public IWindow
{
public:
	MayaWindow( );
	virtual ~MayaWindow();

	const char * id() const override;
	const char * title() const override;

	void update() override;
	void close() override;

	void show() override;
	void hide() override;

	const Menus & menus() const override;
	const Regions & regions() const override;

	QMainWindow * window() const;
	virtual void * nativeWindowId() const override;
	virtual void makeFramelessWindow() override;
private:
	QMainWindow * mainWindow_;

	std::string id_;
	Menus menus_;
	Regions regions_;
};

#endif//MAYA_WINDOW_HPP