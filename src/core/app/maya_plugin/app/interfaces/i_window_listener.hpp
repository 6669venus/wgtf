#ifndef I_WINDOW_LISTENER_HPP
#define I_WINDOW_LISTENER_HPP

class IWindowAdapter;

class IWindowListener
{
public:
	virtual void windowShow( IWindowAdapter * window ) {}
	virtual void windowHide( IWindowAdapter * window ) {}
	virtual void windowClosed( IWindowAdapter * window ) {}
};

#endif // I_WINDOW_LISTENER_HPP
