#ifndef MAYA_TAB_REGION_HPP
#define MAYA_TAB_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/layout_tags.hpp"

class QTabWidget;

class MayaTabRegion : public IRegion
{
public:
	MayaTabRegion( QTabWidget & qTabWidget );

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	QTabWidget & qTabWidget_;
	LayoutTags tags_;
};

#endif//MAYA_TAB_REGION_HPP