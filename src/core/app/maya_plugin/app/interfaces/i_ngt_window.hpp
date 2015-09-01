#ifndef I_NGT_WINDOW_HPP
#define I_NGT_WINDOW_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_framework/i_window.hpp"
#include "i_window_listener.hpp"

class INGTWindow : public Implements< IWindow >
{
public:
	virtual ~INGTWindow() {}

	virtual void * nativeWindowId() const = 0;
	virtual void makeFramelessWindow() = 0;
    virtual void addListener( IWindowListener & listener ) = 0;
    virtual void removeListener( IWindowListener & listener ) = 0;
};

#endif I_NGT_WINDOW_HPP
