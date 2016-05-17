#ifndef I_WINDOW_LISTENER_HPP
#define I_WINDOW_LISTENER_HPP

namespace wgt
{
class IWindowAdapter;

class IWindowListener
{
public:
	virtual void windowShown( IWindowAdapter * window ) {}
	virtual void windowHidden( IWindowAdapter * window ) {}
	virtual void windowClosed( IWindowAdapter * window ) {}
};

}
#endif // I_WINDOW_LISTENER_HPP

