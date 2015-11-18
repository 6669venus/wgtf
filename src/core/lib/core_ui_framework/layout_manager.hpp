#ifndef LAYOUT_MANAGER_HPP
#define LAYOUT_MANAGER_HPP
#include "core_ui_framework/i_ui_application.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

class IAction;
class IMenu;
class IRegion;
class IView;
class IWindow;
struct LayoutHint;

class LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();

	void update() const;

	void addAction( IAction & action );
	void addMenu( IMenu & menu );
	void addView( IView & view );
	void addWindow( IWindow & window );

	void removeAction( IAction & action );
	void removeMenu( IMenu & menu );
	void removeView( IView & view );
	void removeWindow( IWindow & window );

	const Windows & windows() const;

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};

#endif//LAYOUT_MANAGER_HPP