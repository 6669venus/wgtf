#ifndef QT_FRAMEWORK_ADAPTER_HPP
#define QT_FRAMEWORK_ADAPTER_HPP

#include "qt_framework.hpp"


class QtFrameworkAdapter
	: public QtFramework
{
public:
	QtFrameworkAdapter();
	virtual ~QtFrameworkAdapter();
	virtual QmlWindow * createQmlWindow() override;
};

#endif
