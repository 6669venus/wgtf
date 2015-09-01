#ifndef QT_APPLICATION_ADAPTER_HPP
#define QT_APPLICATION_ADAPTER_HPP

#include "core_dependency_system/i_interface.hpp"

#include "qt_application.hpp"
#include "i_application_adapter.hpp"
#include "i_application_listener.hpp"

class QtApplicationAdapter : public QtApplication, public Implements< IApplicationAdapter >
{
public:
	QtApplicationAdapter();
	virtual ~QtApplicationAdapter();

	int startApplication() override;
	void processEvents();
	void addListener( IApplicationListener * listener ) override;
	void removeListener( IApplicationListener * listener ) override;

private:
	std::vector< IApplicationListener * >	listeners_;
};

#endif//QT_APPLICATION_ADAPTER_HPP
