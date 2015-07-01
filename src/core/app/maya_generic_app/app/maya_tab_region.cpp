#include "maya_tab_region.hpp"
#include "qt_common/i_qt_framework.hpp"

#include "ui_framework/i_view.hpp"

#include <QtGui/QTabWidget>
#include <QtCore/QVariant>

MayaTabRegion::MayaTabRegion( IQtFramework & qtFramework, QTabWidget & qTabWidget )
	: qtFramework_( qtFramework )
	, qTabWidget_( qTabWidget )
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
	auto qWidget = qtFramework_.toQWidget( view );
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