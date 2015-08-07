#include "qt_tab_region.hpp"
#include "i_qt_framework.hpp"

#include "core_ui_framework/i_view.hpp"

#include <QTabWidget>
#include <QVariant>

QtTabRegion::QtTabRegion( IQtFramework & qtFramework, QTabWidget & qTabWidget )
	: qtFramework_( qtFramework )
	, qTabWidget_( qTabWidget )
{
	qTabWidget_.setVisible( false );

	auto layoutTagsProperty = qTabWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}
}

const LayoutTags & QtTabRegion::tags() const
{
	return tags_;
}

void QtTabRegion::addView( IView & view )
{
	auto qWidget = qtFramework_.toQWidget( view );
	if (qWidget == nullptr)
	{
		return;
	}

	qTabWidget_.addTab( qWidget, view.title() );
	qTabWidget_.setVisible( true );
}

void QtTabRegion::removeView( IView & view )
{
	// TODO
}