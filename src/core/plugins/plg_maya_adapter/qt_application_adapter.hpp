#ifndef QT_APPLICATION_ADAPTER_HPP
#define QT_APPLICATION_ADAPTER_HPP

#include "qt_application.hpp"

class QtApplicationAdapter : public QtApplication
{
public:
	QtApplicationAdapter();
	virtual ~QtApplicationAdapter();

	int startApplication() override;
	void processEvents();
	void registerListener( IApplicationListener * listener );
	void deregisterListener( IApplicationListener * listener );

private:
	std::vector< IApplicationListener * >	listeners_;
};

#endif//QT_APPLICATION_ADAPTER_HPP
