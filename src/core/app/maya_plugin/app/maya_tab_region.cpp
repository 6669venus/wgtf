#include "maya_tab_region.hpp"
#include "ngt_widget_helper.hpp"

#include "ui_framework/i_view.hpp"

#include <QtGui/QTabWidget>
#include <QtCore/QVariant>

MayaTabRegion::MayaTabRegion( QTabWidget & qTabWidget )
	: qTabWidget_( qTabWidget )
{
	qTabWidget_.setVisible( false );

	auto layoutTagsProperty = qTabWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.constBegin(); it != tags.constEnd(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}
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