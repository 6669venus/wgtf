#include "qt_dock_region.hpp"
#include "i_qt_framework.hpp"

#include "ui_framework/i_view.hpp"

#include <QDockWidget>
#include <QLayout>
#include <QMainWindow>
#include <QVariant>

QtDockRegion::QtDockRegion( IQtFramework & qtFramework, 
						   QMainWindow & qMainWindow, QDockWidget & qDockWidget )
	: qtFramework_( qtFramework )
	, qMainWindow_( qMainWindow )
	, qDockWidget_( qDockWidget )
{
	qDockWidget_.setVisible( false );

	auto layoutTagsProperty = qDockWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}
}

const LayoutTags & QtDockRegion::tags() const
{
	return tags_;
}

void QtDockRegion::addView( IView & view )
{
	auto qWidget = qtFramework_.toQWidget( view );
	if (qWidget == nullptr)
	{
		return;
	}

	qMainWindow_.layout()->addWidget( qWidget );

	auto qDockWidget = new QDockWidget( view.title() );
	qDockWidget->setWidget( qWidget );
	qMainWindow_.tabifyDockWidget( &qDockWidget_, qDockWidget );
}

void QtDockRegion::removeView( IView & view )
{
	// TODO
}