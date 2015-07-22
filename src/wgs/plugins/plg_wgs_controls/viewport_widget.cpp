#include "viewport_widget.hpp"
#include <QtPlugin>
#include <QtGui>

WGSViewport::WGSViewport(QWidget *parent)
	: QWidget( parent )
{
}

void WGSViewport::paintEvent( QPaintEvent * )
{
	// Temporary fill for debugging
	QPainter painter(this);
	painter.fillRect(QRect(QPoint(0, 0), size()), QColor(64, 64, 128));
}

WGSViewportWidget::WGSViewportWidget( QObject *parent )
	: QObject( parent )
{
	initialized = false;
}

void WGSViewportWidget::initialize( QDesignerFormEditorInterface* )
{
	if( initialized )
		return;

	initialized = true;
}

bool WGSViewportWidget::isInitialized() const
{
	return initialized;
}

QWidget *WGSViewportWidget::createWidget( QWidget *parent )
{
	return new WGSViewport( parent );
}

QString WGSViewportWidget::name() const
{
	return "WGSViewport";
}

QString WGSViewportWidget::group() const
{
	return "WGS Custom Widgets";
}

QIcon WGSViewportWidget::icon() const
{
	return QIcon();
}

QString WGSViewportWidget::toolTip() const
{
	return "";
}

QString WGSViewportWidget::whatsThis() const
{
	return "";
}

bool WGSViewportWidget::isContainer() const
{
	return false;
}

QString WGSViewportWidget::domXml() const
{
	return "<ui language=\"c++\">\n"
		" <widget class=\"WGSViewport\" name=\"WGSViewport\">\n"
		"  <property name=\"geometry\">\n"
		"   <rect>\n"
		"    <x>0</x>\n"
		"    <y>0</y>\n"
		"    <width>100</width>\n"
		"    <height>100</height>\n"
		"   </rect>\n"
		"  </property>\n"
		" </widget>\n"
		"</ui>\n";
}

QString WGSViewportWidget::includeFile() const
{
	return "viewport_widget.h";
}