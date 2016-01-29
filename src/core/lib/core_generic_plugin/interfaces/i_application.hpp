#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP

#include "wg_types/event.hpp"

class IApplication
{
public:
	virtual ~IApplication() {}

	virtual int startApplication() = 0;
	virtual void quitApplication() = 0;

	PUBLIC_EVENT( IApplication, StartUp )
	PUBLIC_EVENT( IApplication, Update )
};

#endif //I_APPLICATION_HPP

