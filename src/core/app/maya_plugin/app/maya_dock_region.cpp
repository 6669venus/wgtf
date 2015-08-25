#include "maya_dock_region.hpp"
#include "core_ui_framework/i_view.hpp"
#include "ngt_widget_helper.hpp"

#include <QtGui/QDockWidget>
#include <QtGui/QLayout>
#include <QtGui/QMainWindow>
#include <QtCore/QVariant>

MayaDockRegion::MayaDockRegion( QMainWindow & qMainWindow,
		QDockWidget & qDockWidget )
	: qMainWindow_( qMainWindow )
	, qDockWidget_( qDockWidget )
{
	//TODO: Implement the layout properly if possible
	tags_.tags_.push_back( "default" );
}

const LayoutTags & MayaDockRegion::tags() const
{
	return tags_;
}

void MayaDockRegion::addView( IView & view )
{
	auto qWidget = NGTWidgetHelper::create( view.nativeWindowId() );
	if (qWidget == nullptr)
	{
		return;
	}
	qWidget->show();
	qMainWindow_.centralWidget()->layout()->addWidget( qWidget );
	qWidget->setSizePolicy( qDockWidget_.sizePolicy() );
	qWidget->setMinimumSize( qDockWidget_.minimumSize() );
	qWidget->setMaximumSize( qDockWidget_.maximumSize() );
	qWidget->setSizeIncrement( qDockWidget_.sizeIncrement() );
	qWidget->setBaseSize( qDockWidget_.baseSize() );
	qWidget->resize( qWidget->baseSize() );

	auto qDockWidget = new QDockWidget( view.title() );
	qMainWindow_.tabifyDockWidget( &qDockWidget_, qDockWidget );
	qDockWidget->setWidget( qWidget );
	qDockWidget->setFloating( qDockWidget_.isFloating() );
	qDockWidget->setFeatures( qDockWidget_.features() );
	qDockWidget->setAllowedAreas( qDockWidget_.allowedAreas() );
}

void MayaDockRegion::removeView( IView & view )
{
	// TODO
}