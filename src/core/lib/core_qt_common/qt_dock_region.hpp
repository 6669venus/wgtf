#ifndef QT_DOCK_REGION_HPP
#define QT_DOCK_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include <map>
#include <memory>

class IQtFramework;
class QDockWidget;
class QtWindow;

class QtDockRegion : public IRegion
{
public:
	QtDockRegion( IQtFramework & qtFramework, QtWindow & qtWindow, QDockWidget & qDockWidget );

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	IQtFramework & qtFramework_;
	QtWindow & qtWindow_;
	QDockWidget & qDockWidget_;
	LayoutTags tags_;
	typedef std::pair< std::unique_ptr< QDockWidget >, std::unique_ptr< IAction > > DockData;
	std::map< IView*, DockData > dockWidgetMap_;
};

#endif//QT_DOCK_REGION_HPP