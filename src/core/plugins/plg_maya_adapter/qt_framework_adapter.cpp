#include "qt_framework_adapter.hpp"
#include "qt_window_adapter.hpp"

QtFrameworkAdapter::QtFrameworkAdapter()
	:QtFramework()
{
}

QtFrameworkAdapter::~QtFrameworkAdapter()
{
}

QtWindow * QtFrameworkAdapter::createQtWindow( QIODevice & source )
{
	return new QtWindowAdapter( *this, source );
}
