#include "maya_dock_region.hpp"
#include "ui_framework/i_view.hpp"
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
	qDockWidget_.setVisible( false );

	auto layoutTagsProperty = qDockWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.constBegin(); it != tags.constEnd(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}
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