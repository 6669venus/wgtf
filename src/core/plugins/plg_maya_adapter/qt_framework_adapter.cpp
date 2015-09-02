#include "qt_framework_adapter.hpp"
#include "qml_window_adapter.hpp"
#include "qt_window_adapter.hpp"

QtFrameworkAdapter::QtFrameworkAdapter()
	:QtFramework()
{
}

QtFrameworkAdapter::~QtFrameworkAdapter()
{
}

QmlWindow * QtFrameworkAdapter::createQmlWindow()
{
	return new QmlWindowAdapter( *this, *qmlEngine() );
}

QtWindow * QtFrameworkAdapter::createQtWindow( QIODevice & source )
{
	return new QtWindowAdapter( *this, source );
}
