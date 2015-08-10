#ifndef MAYA_DOCK_REGION_HPP
#define MAYA_DOCK_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/layout_tags.hpp"

class QDockWidget;
class QMainWindow;

class MayaDockRegion : public IRegion
{
public:
	MayaDockRegion( QMainWindow & qMainWindow, QDockWidget & qDockWidget );

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	QMainWindow & qMainWindow_;
	QDockWidget & qDockWidget_;
	LayoutTags tags_;
};

#endif//MAYA_DOCK_REGION_HPP