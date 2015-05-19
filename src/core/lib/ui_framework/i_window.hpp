#ifndef I_WINDOW_HPP
#define I_WINDOW_HPP

struct LayoutHint;
class IAction;
class IView;

class IWindow
{
public:
	virtual ~IWindow() {}

	virtual const char * id() = 0;

	virtual void update() = 0;
	virtual void close() = 0;

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual void addFrame( IView & frame, LayoutHint & hint ) = 0;
	virtual void addPanel( IView & panel, LayoutHint & hint ) = 0;
	virtual void addAction( IAction & action, const char * path, const char * shortcut ) = 0;
};

#endif I_WINDOW_HPP