#include "layout_manager.hpp"
#include "layout_hint.hpp"

#include "i_action.hpp"
#include "i_menu.hpp"
#include "i_region.hpp"
#include "i_view.hpp"
#include "i_window.hpp"

#include <algorithm>

class LayoutManager::Impl
{
public:
	IMenu * findBestMenu( IWindow & window, const char * path, size_t& bestMenuPathLen )
	{
		IMenu * bestMenu = nullptr;
		auto & windowMenus = window.menus();

		std::vector< IMenu * > menus( windowMenus.size() + menus_.size() );
		
		auto it = std::transform( windowMenus.begin(), windowMenus.end(), menus.begin(), []( const std::unique_ptr< IMenu > & menu ) 
		{ 
			return menu.get(); 
		} );

		auto windowId = window.id();
		if (windowId == nullptr)
		{
			windowId = "";
		}

		std::copy_if( menus_.begin(), menus_.end(), it, [&]( IMenu * menu )
		{
			const char * menuWindowId = menu->windowId();
			if (menuWindowId == nullptr)
			{
				menuWindowId = "";
			}

			return strcmp(windowId, menuWindowId) == 0;
		} );

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
				bestMenu = menu;
				bestMenuPathLen = menuPathLen;
			}
		}
		return bestMenu;
	}

	void addAction( IWindow & window, IAction & action, const char * path, IMenu *& menu )
	{
		IMenu * bestMenu = nullptr;
		size_t bestMenuPathLen = 0;
		bestMenu = findBestMenu( window, path, bestMenuPathLen );

		if (menu == bestMenu)
		{
			return;
		}

		if (menu != nullptr)
		{
			menu->removeAction( action );
		}

		menu = bestMenu;

		if (menu != nullptr)
		{
			menu->addAction( action, path + bestMenuPathLen );
		}
	}

	IRegion * findBestRegion( IWindow & window, const LayoutHint & hint )
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
		return bestRegion;
	}

	void addView( IWindow & window, IView & view, IRegion *& region )
	{
		IRegion * bestRegion = findBestRegion( window, view.hint() );
		if (bestRegion == nullptr)
		{
			bestRegion = findBestRegion( window, LayoutHint( "default" ) );
		}

		if (region == bestRegion)
		{
			return;
		}

		if (region != nullptr)
		{
			region->removeView( view );
		}

		region = bestRegion;

		if (region != nullptr)
		{
			region->addView( view );
		}
	}

	void addActions( IWindow & window )
	{
		auto windowId = window.id();
		if (windowId == nullptr)
		{
			windowId = "";
		}

		for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
		{
			IAction * action = actionIt->first;
			IMenu * menu = actionIt->second;

			auto actionWindowId = action->windowId();
			if (actionWindowId == nullptr)
			{
				actionWindowId = "";
			}

			if (strcmp( windowId, actionWindowId ) != 0)
			{
				continue;
			}

			addAction( window, *action, action->path(), menu );

			actions_[ action ] = menu;
		}
	}

	void addViews( IWindow & window )
	{
		auto windowId = window.id();
		if (windowId == nullptr)
		{
			windowId = "";
		}

		for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
		{
			IView * view = viewIt->first;
			IRegion * region = viewIt->second;

			auto viewWindowId = view->windowId();
			if (viewWindowId == nullptr)
			{
				viewWindowId = "";
			}

			if (strcmp( windowId, viewWindowId ) != 0)
			{
				continue;
			}

			addView( window, *view, region );

			views_[ view ] = region;
		}
	}

	void removeActions( IWindow & window )
	{
		auto windowId = window.id();
		if (windowId == nullptr)
		{
			windowId = "";
		}

		for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
		{
			IAction * action = actionIt->first;
			IMenu * menu = actionIt->second;

			auto actionWindowId = action->windowId();
			if (actionWindowId == nullptr)
			{
				actionWindowId = "";
			}

			if (strcmp( windowId, actionWindowId ) != 0)
			{
				continue;
			}

			if (menu == nullptr)
			{
				continue;
			}

			menu->removeAction( *action );

			actions_[action] = nullptr;
		}
	}

	void removeViews( IWindow & window )
	{
		auto windowId = window.id();
		if (windowId == nullptr)
		{
			windowId = "";
		}

		for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
		{
			IView * view = viewIt->first;
			IRegion * region = viewIt->second;

			auto viewWindowId = view->windowId();
			if (viewWindowId == nullptr)
			{
				viewWindowId = "";
			}

			if (strcmp( windowId, viewWindowId ) != 0)
			{
				continue;
			}

			if (region == nullptr)
			{
				continue;
			}

			region->removeView( *view );

			views_[view] = nullptr;
		}
	}

	std::map< IAction *, IMenu * > actions_;
	std::map< IView *, IRegion * > views_;
	std::vector< IMenu * > menus_;
	Windows windows_;
};

LayoutManager::LayoutManager()
	: impl_( new Impl )
{

}

LayoutManager::~LayoutManager()
{

}

void LayoutManager::update() const
{
	/* TODO: remove the need for this */
	for (auto & view : impl_->views_)
	{
		view.first->update();
	}

	for (auto & window : impl_->windows_)
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

	IMenu * menu = nullptr;

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		impl_->addAction( *windowIt->second, action, path, menu );
	}

	impl_->actions_[ &action ] = menu;
}

void LayoutManager::addMenu( IMenu & menu )
{
	auto menuIt = std::find( impl_->menus_.begin(), impl_->menus_.end(), &menu );
	if (menuIt != impl_->menus_.end())
	{
		return;
	}

	impl_->menus_.push_back( &menu );

	const char * windowId = nullptr;//menu.windowId();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		impl_->addActions( *windowIt->second );
	}
}

void LayoutManager::addView( IView & view )
{
	auto windowId = view.windowId();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	IRegion * region = nullptr;

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		impl_->addView( *windowIt->second, view, region );
	}

	impl_->views_[ &view ] = region;
}

void LayoutManager::addWindow( IWindow & window )
{
	auto windowId = window.id();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		//error?
		return;
	}

	impl_->windows_[ windowId ] = &window;

	impl_->addActions( window );
	impl_->addViews( window );
}

void LayoutManager::removeAction( IAction & action )
{
	auto actionIt = impl_->actions_.find( &action );
	if (actionIt == impl_->actions_.end())
	{
		return;
	}

	IMenu * menu = actionIt->second;
	if (menu == nullptr)
	{
		return;
	}

	menu->removeAction( action );
	impl_->actions_.erase( actionIt );
}

void LayoutManager::removeMenu( IMenu & menu )
{
	auto menuIt = std::find( impl_->menus_.begin(), impl_->menus_.end(), &menu );
	if (menuIt == impl_->menus_.end())
	{
		return;
	}

	impl_->menus_.erase( menuIt );

	const char * windowId = menu.windowId();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		impl_->removeActions( *windowIt->second );
	}
}

void LayoutManager::removeView( IView & view )
{
	auto viewIt = impl_->views_.find( &view );
	if (viewIt == impl_->views_.end())
	{
		return;
	}

	IRegion * region = viewIt->second;
	if (region == nullptr)
	{
		return;
	}

	region->removeView( view );
	impl_->views_.erase( viewIt );
}

void LayoutManager::removeWindow( IWindow & window )
{
	auto windowId = window.id();
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt == impl_->windows_.end())
	{
		return;
	}

	impl_->removeActions( window );
	impl_->removeViews( window );

	impl_->windows_.erase( windowId );
}


const Windows & LayoutManager::windows() const
{
	return impl_->windows_;
}
