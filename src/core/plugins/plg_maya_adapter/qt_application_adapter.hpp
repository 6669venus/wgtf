#ifndef QT_APPLICATION_ADAPTER_HPP
#define QT_APPLICATION_ADAPTER_HPP

#include "qt_application.hpp"

class QtApplicationAdapter : public QtApplication
{
public:
	QtApplicationAdapter();
	virtual ~QtApplicationAdapter();

	int startApplication() override;
	void processEvents() override;
	void registerListener( IApplicationListener * listener );
	void deregisterListener( IApplicationListener * listener );
};

#endif//QT_APPLICATION_ADAPTER_HPP
