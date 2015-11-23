#include "layout_manager.hpp"
#include "layout_hint.hpp"

#include "i_action.hpp"
#include "i_menu.hpp"
#include "i_region.hpp"
#include "i_view.hpp"
#include "i_window.hpp"

#include <algorithm>

namespace 
{
	const char * safe_str( const char * str )
	{
		return str ? str : "";
	}
}

IMenu * LayoutManager::findBestMenu( IWindow & window, const char * path )
{
	// create a collection of all menus associated with this window.
	auto & windowMenus = window.menus();
	std::vector< IMenu * > menus( windowMenus.size() + dynamicMenus_.size() );
		
	// copy all the menus owned by the window
	auto it = std::transform( windowMenus.begin(), windowMenus.end(), menus.begin(), []( const std::unique_ptr< IMenu > & menu ) 
	{ 
		return menu.get(); 
	} );

	// copy all the menus dynamically registered for this window
	auto windowId = safe_str( window.id() );
	it = std::copy_if( dynamicMenus_.begin(), dynamicMenus_.end(), it, [&]( IMenu * menu )
	{
		auto menuWindowId = safe_str( menu->windowId() );
		return strcmp(windowId, menuWindowId) == 0;
	} );

	// resize the collection to the actual number of menus that were collected
	menus.erase( it, menus.end() );

	// find the menu who's path is the longest substring of the path passed in
	IMenu * bestMenu = nullptr;
	size_t bestMenuPathLen = 0;
	for (auto & menu : menus)
	{
		auto menuPath = menu->path();
		auto menuPathLen = strlen( menuPath );
		if (bestMenu != nullptr)
		{
			if (menuPathLen < bestMenuPathLen)
			{
				continue;
			}
		}
		if (strncmp( path, menuPath, menuPathLen) != 0)
		{
			continue;
		}
		if (menuPathLen == 0 || path[menuPathLen] == '.' || path[menuPathLen] == '\0')
		{
			bestMenu = menu;
			bestMenuPathLen = menuPathLen;
		}
	}
	return bestMenu;
}

IRegion * LayoutManager::findBestRegion( IWindow & window, const LayoutHint & hint )
{
	// find the region who's tags have the greatest correlation to the passed in hint
	IRegion * bestRegion = nullptr;
	float bestRegionScore = 0.f;
	auto & regions = window.regions();
	for (auto & region : regions)
	{
		auto & regionTags = region->tags();
		auto regionScore = hint.match( regionTags );
		if (regionScore > bestRegionScore)
		{
			bestRegion = region.get();
			bestRegionScore = regionScore;
		}
	}
	return bestRegion;
}

void LayoutManager::addAction( IAction & action, IWindow * window )
{
	auto menu = actions_[ &action ];

	IMenu * bestMenu = nullptr;
	if (window != nullptr)
	{
		bestMenu = findBestMenu( *window, safe_str( action.path() ) );
	}

	// if the best menu for this action is already the menu it is assigned to, early out
	if (menu == bestMenu)
	{
		return;
	}

	if (menu != nullptr)
	{
		// the menu the action is assigned to is no longer the best one so remove the action
		menu->removeAction( action );
	}

	menu = bestMenu;

	if (menu != nullptr)
	{
		// add the action to the best menu
		menu->addAction( action );
	}

	// assign the best menu to the action, even if this menu is null
	actions_[ &action ] = menu;
}

void LayoutManager::addView( IView & view, IWindow * window )
{
	auto region = views_[ &view ];

	IRegion * bestRegion = nullptr;
	if (window != nullptr)
	{
		bestRegion = findBestRegion( *window, view.hint() );
		if (bestRegion == nullptr)
		{
			bestRegion = findBestRegion( *window, LayoutHint( "default" ) );
		}
	}

	// if the best region for this view is already the region it is assigned to, early out
	if (region == bestRegion)
	{
		return;
	}

	if (region != nullptr)
	{
		// the region the view is assigned to is no longer the best one so remove the view
		region->removeView( view );
	}

	region = bestRegion;

	if (region != nullptr)
	{
		// add the view to the best region
		region->addView( view );
	}

	// assign the best region to the view, even if this region is null
	views_[ &view ] = region;
}

void LayoutManager::refreshActions( IWindow * window )
{
	// go through every action associated with this window and try to re add it.
	// this will move the action between menus where appropriate
	for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
	{
		auto action = actionIt->first;
		if (getWindow( action->windowId() ) != window)
		{
			continue;
		}
			
		addAction( *action, window );
	}
}

void LayoutManager::refreshViews( IWindow * window )
{
	// go through every view associated with this window and try to re add it.
	// this will move the view between regions where appropriate
	for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
	{
		auto view = viewIt->first;
		if (getWindow( view->windowId() ) != window)
		{
			continue;
		}

		addView( *view, window );
	}
}

IWindow * LayoutManager::getWindow( const char * windowId )
{
	auto windowIt = windows_.find( safe_str( windowId ) );
	if (windowIt == windows_.end())
	{
		return nullptr;
	}
	return windowIt->second;
}

LayoutManager::LayoutManager()
{

}

LayoutManager::~LayoutManager()
{

}

void LayoutManager::update() const
{
	/* TODO: remove the need for this */
	for (auto & view : views_)
	{
		view.first->update();
	}

	for (auto & window : windows_)
	{
		window.second->update();
	}
	/* ------------------------------ */
}

void LayoutManager::addAction( IAction & action )
{
	auto window = getWindow( action.windowId() );
	addAction( action, window );
}

void LayoutManager::addMenu( IMenu & menu )
{
	dynamicMenus_.push_back( &menu );

	// after adding a menu to a window we need to recheck all the actions attached to the
	// window in case they need to be moved
	auto window = getWindow( menu.windowId() );
	if (window != nullptr)
	{
		refreshActions( window );
	}
}

void LayoutManager::addView( IView & view )
{
	auto window = getWindow( view.windowId() );
	addView( view, window );
}

void LayoutManager::addWindow( IWindow & window )
{
	auto windowId = safe_str( window.id() );

	auto windowIt = windows_.find( windowId );
	if (windowIt != windows_.end())
	{
		//error?
		return;
	}

	windows_[ windowId ] = &window;

	// after adding a window we need to reevaluate all the actions and views that we registered
	// against this window as they now all need to be added to the UI
	refreshActions( &window );
	refreshViews( &window );
}

void LayoutManager::removeAction( IAction & action )
{
	auto actionIt = actions_.find( &action );
	if (actionIt == actions_.end())
	{
		return;
	}

	auto menu = actionIt->second;
	if (menu == nullptr)
	{
		return;
	}

	menu->removeAction( action );
	actions_.erase( actionIt );
}

void LayoutManager::removeMenu( IMenu & menu )
{
	auto menuIt = std::find( dynamicMenus_.begin(), dynamicMenus_.end(), &menu );
	if (menuIt == dynamicMenus_.end())
	{
		return;
	}

	dynamicMenus_.erase( menuIt );

	// removing a menu will also require us to reevaluate all the actions associated with the menu's window
	auto window = getWindow( menu.windowId() );
	if (window != nullptr)
	{
		refreshActions( window );
	}
}

void LayoutManager::removeView( IView & view )
{
	auto viewIt = views_.find( &view );
	if (viewIt == views_.end())
	{
		return;
	}

	auto region = viewIt->second;
	if (region == nullptr)
	{
		return;
	}

	region->removeView( view );
	views_.erase( viewIt );
}

void LayoutManager::removeWindow( IWindow & window )
{
	auto windowId = safe_str( window.id() );

	auto windowIt = windows_.find( windowId );
	if (windowIt == windows_.end())
	{
		return;
	}

	windows_.erase( windowId );

	// when a window is removed we need to reevaluate all the actions and views that used to be associated
	// with the window so that we can remove them from the UI
	refreshActions( nullptr );
	refreshViews( nullptr );
}

const Windows & LayoutManager::windows() const
{
	return windows_;
}
