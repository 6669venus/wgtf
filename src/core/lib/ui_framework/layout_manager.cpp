#include "layout_manager.hpp"
#include "layout_hint.hpp"

#include "i_action.hpp"
#include "i_menu.hpp"
#include "i_region.hpp"
#include "i_view.hpp"
#include "i_window.hpp"

#include <algorithm>

namespace LayoutManager_Locals
{
	bool addAction( IWindow & window, IAction & action, const char * path )
	{
		IMenu * bestMenu = nullptr;
		size_t bestMenuPathLen = 0;

		auto & menus = window.menus();
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
			if (strncmp( path, menuPath, menuPathLen) == 0)
			{
				bestMenu = menu.get();
				bestMenuPathLen = menuPathLen;
			}
		}

		if (bestMenu == nullptr)
		{
			return false;
		}

		bestMenu->addAction( action, path + bestMenuPathLen );
		return true;
	}

	bool addView( IWindow & window, IView & view, const LayoutHint & hint )
	{
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

		if (bestRegion == nullptr)
		{
			return false;
		}

		bestRegion->addView( view );
		return true;
	}

	bool addView( IWindow & window, IView & view )
	{
		if (addView( window, view, view.hint() ))
		{
			return true;
		}

		return addView( window, view, LayoutHint( "default" ) );
	}
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
	for (auto & views : views_)
	{
		for (auto & view : views.second)
		{
			view->update();
		}
	}

	for (auto & window : windows_)
	{
		window.second->update();
	}
	/* ------------------------------ */
}

void LayoutManager::addAction( IAction & action )
{
	const char * windowId = action.windowId();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	const char * path = action.path();
	if (path == nullptr)
	{
		path = "";
	}

	auto windowIt = windows_.find( windowId );
	if (windowIt != windows_.end())
	{
		LayoutManager_Locals::addAction( *windowIt->second, action, path );
	}

	actions_[ windowId ].emplace_back( &action );
}

void LayoutManager::addView( IView & view )
{
	auto windowId = view.windowId();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = windows_.find( windowId );
	if (windowIt != windows_.end())
	{
		LayoutManager_Locals::addView( *windowIt->second, view );
	}

	views_[ windowId ].emplace_back( &view );
}

void LayoutManager::addWindow( IWindow & window )
{
	auto windowId = window.id();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = windows_.find( windowId );
	if (windowIt != windows_.end())
	{
		//error?
		return;
	}

	auto viewIt = views_.find( windowId );
	if (viewIt != views_.end())
	{
		for (auto & view : viewIt->second)
		{
			LayoutManager_Locals::addView( window, *view );
		}
	}

	auto actionIt = actions_.find( windowId );
	if (actionIt != actions_.end())
	{
		for (auto & action : actionIt->second)
		{
			LayoutManager_Locals::addAction( window, *action, action->path() );
		}
	}

	windows_[ windowId ] = &window;
}

void LayoutManager::removeAction( IAction & action )
{
	// TODO : Not yet implemented
}

void LayoutManager::removeView( IView & view )
{
	// TODO : Not yet implemented
}

void LayoutManager::removeWindow( IWindow & window )
{
	// TODO : Not yet implemented
}