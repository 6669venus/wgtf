#include "ngt_widget_helper.hpp"
#include "qwinhost.h"

namespace NGTWidgetHelper
{

QWidget * create( void * nativeWindowId )
{
	if ( nativeWindowId == nullptr )
	{
		return nullptr;
	}

	auto widget = new QWinHost();
	widget->setWindow( reinterpret_cast<HWND>( nativeWindowId ) );
	return widget;
}

}
