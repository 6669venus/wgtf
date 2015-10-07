#include "qt_dock_region.hpp"
#include "i_qt_framework.hpp"

#include "core_ui_framework/i_view.hpp"

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
	auto findIt = dockWidgetMap_.find( &view );
	if (findIt != dockWidgetMap_.end())
	{
		// already added into the dockWidget
		return;
	}
	// IView will not control qWidget's life-cycle after this call.
	auto qWidget = qtFramework_.toQWidget( view );
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
	dockWidgetMap_[ &view ] = qDockWidget;
}

void QtDockRegion::removeView( IView & view )
{
	auto findIt = dockWidgetMap_.find( &view );
	if (findIt == dockWidgetMap_.end())
	{
		return;
	}
	
	//TODO: save dockWidget state
	auto dockWidget = findIt->second;
	dockWidgetMap_.erase( &view );
	assert( dockWidget != nullptr );
	dockWidget->setWidget( nullptr );
	qMainWindow_.removeDockWidget( dockWidget );
	// call this function to let IView control the qWidget's life-cycle again.
	qtFramework_.retainQWidget( view );
	delete dockWidget;
	dockWidget = nullptr;
}