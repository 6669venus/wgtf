#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP


class IApplicationListener
{
public:
	virtual void applicationStarted() {}
	virtual void applicationStopped() {}
};

class IApplication
{
public:
	virtual ~IApplication() {}

	virtual int startApplication() = 0;
};

#endif //I_APPLICATION_HPP

