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

class LayoutManager::Impl
{
public:
	IMenu * findBestMenu( IWindow & window, const char * path )
	{
		IMenu * bestMenu = nullptr;
		auto & windowMenus = window.menus();

		std::vector< IMenu * > menus( windowMenus.size() + dynamicMenus_.size() );
		
		auto it = std::transform( windowMenus.begin(), windowMenus.end(), menus.begin(), []( const std::unique_ptr< IMenu > & menu ) 
		{ 
			return menu.get(); 
		} );

		auto windowId = safe_str( window.id() );

		it = std::copy_if( dynamicMenus_.begin(), dynamicMenus_.end(), it, [&]( IMenu * menu )
		{
			auto menuWindowId = safe_str( menu->windowId() );

			return strcmp(windowId, menuWindowId) == 0;
		} );

		menus.erase( it, menus.end() );

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
			if (strncmp( path, menuPath, menuPathLen) == 0)
			{
				bestMenu = menu;
				bestMenuPathLen = menuPathLen;
			}
		}
		return bestMenu;
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

	void addAction( IAction & action, IWindow * window )
	{
		auto actionIt = actions_.find( &action );
		auto menu = actions_[ &action ];

		IMenu * bestMenu = nullptr;
		if (window != nullptr)
		{
			bestMenu = findBestMenu( *window, safe_str( action.path() ) );
		}

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
			menu->addAction( action );
		}

		actions_[ &action ] = menu;
	}

	void addView( IView & view, IWindow * window )
	{
		auto viewIt = views_.find( &view );
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

		views_[ &view ] = region;
	}

	void refreshActions( IWindow & window )
	{
		for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
		{
			auto action = actionIt->first;
			if (getWindow( action->windowId() ) != &window)
			{
				continue;
			}
			
			addAction( *action, &window );
		}
	}

	void refreshViews( IWindow & window )
	{
		for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
		{
			auto view = viewIt->first;
			if (getWindow( view->windowId() ) != &window)
			{
				continue;
			}

			addView( *view, &window );
		}
	}

	void removeActions( IWindow & window )
	{
		for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
		{
			auto action = actionIt->first;
			if (getWindow( action->windowId() ) != &window)
			{
				continue;
			}

			auto menu = actionIt->second;
			if (menu == nullptr)
			{
				continue;
			}

			menu->removeAction( *action );
			actionIt->second = nullptr;
		}
	}

	void removeViews( IWindow & window )
	{
		for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
		{
			auto view = viewIt->first;
			if (getWindow( view->windowId() ) != &window)
			{
				continue;
			}

			auto region = viewIt->second;
			if (region == nullptr)
			{
				continue;
			}			

			region->removeView( *view );
			viewIt->second = nullptr;
		}
	}

	IWindow * getWindow( const char * windowId )
	{
		auto windowIt = windows_.find( safe_str( windowId ) );
		if (windowIt == windows_.end())
		{
			return nullptr;
		}
		return windowIt->second;
	}


	std::map< IAction *, IMenu * > actions_;
	std::map< IView *, IRegion * > views_;
	std::vector< IMenu * > dynamicMenus_;
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
	auto window = impl_->getWindow( action.windowId() );
	impl_->addAction( action, window );
}

void LayoutManager::addMenu( IMenu & menu )
{
	impl_->dynamicMenus_.push_back( &menu );

	auto window = impl_->getWindow( menu.windowId() );
	if (window != nullptr)
	{
		impl_->refreshActions( *window );
	}
}

void LayoutManager::addView( IView & view )
{
	auto window = impl_->getWindow( view.windowId() );
	impl_->addView( view, window );
}

void LayoutManager::addWindow( IWindow & window )
{
	auto windowId = safe_str( window.id() );

	auto windowIt = impl_->windows_.find( windowId );
	if (windowIt != impl_->windows_.end())
	{
		//error?
		return;
	}

	impl_->windows_[ windowId ] = &window;

	impl_->refreshActions( window );
	impl_->refreshViews( window );
}

void LayoutManager::removeAction( IAction & action )
{
	auto actionIt = impl_->actions_.find( &action );
	if (actionIt == impl_->actions_.end())
	{
		return;
	}

	auto menu = actionIt->second;
	if (menu == nullptr)
	{
		return;
	}

	menu->removeAction( action );
	impl_->actions_.erase( actionIt );
}

void LayoutManager::removeMenu( IMenu & menu )
{
	auto menuIt = std::find( impl_->dynamicMenus_.begin(), impl_->dynamicMenus_.end(), &menu );
	if (menuIt == impl_->dynamicMenus_.end())
	{
		return;
	}

	impl_->dynamicMenus_.erase( menuIt );

	auto window = impl_->getWindow( menu.windowId() );
	if (window != nullptr)
	{
		impl_->refreshActions( *window );
	}
}

void LayoutManager::removeView( IView & view )
{
	auto viewIt = impl_->views_.find( &view );
	if (viewIt == impl_->views_.end())
	{
		return;
	}

	auto region = viewIt->second;
	if (region == nullptr)
	{
		return;
	}

	region->removeView( view );
	impl_->views_.erase( viewIt );
}

void LayoutManager::removeWindow( IWindow & window )
{
	auto windowId = safe_str( window.id() );

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
