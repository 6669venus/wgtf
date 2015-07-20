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
	virtual void processEvents() = 0;
	virtual void registerListener( IApplicationListener * listener ) = 0;
	virtual void deregisterListener( IApplicationListener * listener ) = 0;
};

#endif //I_APPLICATION_HPP

