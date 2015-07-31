#ifndef VIEWPORT_WIDGET_HPP
#define VIEWPORT_WIDGET_HPP

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/QDesignerExportWidget>
#include <QWidget>

class QDESIGNER_WIDGET_EXPORT WGSViewport : public QWidget
{
	Q_OBJECT

public:
	WGSViewport( QWidget *parent = 0 );

protected:
	void paintEvent( QPaintEvent *event );
};

class WGSViewportWidget : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES( QDesignerCustomWidgetInterface )

public:
	WGSViewportWidget( QObject *parent = 0 );

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget( QWidget *parent );
	void initialize( QDesignerFormEditorInterface *core );

private:
	bool initialized;
};

#endif // VIEWPORT_WIDGET_HPP
