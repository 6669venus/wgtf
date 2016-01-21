#ifndef QT_FRAMEWORK_ADAPTER_HPP
#define QT_FRAMEWORK_ADAPTER_HPP

#include "qt_framework.hpp"


class QtFrameworkAdapter
	: public QtFramework
{
public:
	QtFrameworkAdapter( IComponentContext & contextManager );
	virtual ~QtFrameworkAdapter();
	virtual QtWindow * createQtWindow( QIODevice & source ) override;
};

#endif
