#ifndef MAYA_TAB_REGION_HPP
#define MAYA_TAB_REGION_HPP

#include "ui_framework/i_region.hpp"
#include "ui_framework/layout_tags.hpp"

class IQtFramework;
class QTabWidget;

class MayaTabRegion : public IRegion
{
public:
	MayaTabRegion( IQtFramework & qtFramework, QTabWidget & qTabWidget );

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	IQtFramework & qtFramework_;
	QTabWidget & qTabWidget_;
	LayoutTags tags_;
};

#endif//MAYA_TAB_REGION_HPP