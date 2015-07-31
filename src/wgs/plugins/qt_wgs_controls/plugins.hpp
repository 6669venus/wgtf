#ifndef PLUGINS_HPP
#define PLUGINS_HPP

#include "viewport_widget.hpp"

#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>

class WgsCustomWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
	Q_INTERFACES( QDesignerCustomWidgetCollectionInterface )

public:
	WgsCustomWidgets( QObject *parent = 0 );

	virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
	QList<QDesignerCustomWidgetInterface*> widgets;
};

#endif // PLUGINS_HPP
