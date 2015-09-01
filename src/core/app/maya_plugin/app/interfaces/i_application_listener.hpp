#ifndef I_APPLICATION_LISTENER_HPP
#define I_APPLICATION_LISTENER_HPP

class IApplicationListener
{
public:

	virtual void applicationStarted() = 0;
    virtual void applicationStopped() = 0;
};

#endif // I_APPLICATION_LISTENER_HPP
