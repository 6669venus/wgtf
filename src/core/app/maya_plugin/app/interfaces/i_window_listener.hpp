#ifndef I_WINDOW_LISTENER_HPP
#define I_WINDOW_LISTENER_HPP

class IWindowListener
{
public:

	virtual void windowCreated() = 0;
    virtual void windowDestroyed() = 0;
};

#endif // I_WINDOW_LISTENER_HPP
