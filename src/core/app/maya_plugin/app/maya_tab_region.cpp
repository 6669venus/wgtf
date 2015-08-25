#include "maya_tab_region.hpp"
#include "ngt_widget_helper.hpp"

#include "core_ui_framework/i_view.hpp"

#include <QtGui/QTabWidget>
#include <QtCore/QVariant>

MayaTabRegion::MayaTabRegion( QTabWidget & qTabWidget )
	: qTabWidget_( qTabWidget )
{
	//TODO: Implement the layout properly if possible
	tags_.tags_.push_back( "default" );

}

const LayoutTags & MayaTabRegion::tags() const
{
	return tags_;
}

void MayaTabRegion::addView( IView & view )
{
	auto qWidget = NGTWidgetHelper::create( view.nativeWindowId() );
	if (qWidget == nullptr)
	{
		return;
	}

	qTabWidget_.addTab( qWidget, view.title() );
	qTabWidget_.setVisible( true );
}

void MayaTabRegion::removeView( IView & view )
{
	// TODO
}