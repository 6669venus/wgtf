#include "plugins.hpp"

WgsCustomWidgets::WgsCustomWidgets( QObject *parent )
	: QObject( parent )
{
	widgets.append( new WGSViewportWidget( this ) );
}

QList<QDesignerCustomWidgetInterface*> WgsCustomWidgets::customWidgets() const
{
	return widgets;
}
