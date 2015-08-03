#ifndef LAYOUT_MANAGER_HPP
#define LAYOUT_MANAGER_HPP
#include "core_ui_framework/i_ui_application.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

class IAction;
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
	void addView( IView & view );
	void addWindow( IWindow & window );

	void removeAction( IAction & action );
	void removeView( IView & view );
	void removeWindow( IWindow & window );
	const Windows & windows() const;

private:
	std::map< std::string, std::vector< IAction * > > actions_;
	std::map< std::string, std::vector< IView * > > views_;
	Windows windows_;
};

#endif//LAYOUT_MANAGER_HPP