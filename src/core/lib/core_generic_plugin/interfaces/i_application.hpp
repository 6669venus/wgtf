#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP

#include "core_common/signal.hpp"

class IApplication
{
	typedef Signal<void(void)> SignalVoid;

public:
	virtual ~IApplication() {}

	virtual int startApplication() = 0;
	virtual void quitApplication() = 0;

	SignalVoid onStartUp;
	SignalVoid onUpdate;
};

#endif //I_APPLICATION_HPP

