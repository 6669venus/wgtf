#ifndef I_UI_APPLICATION_HPP
#define I_UI_APPLICATION_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_frameworkd/i_ui_application.hpp"
#include "i_application_listener.hpp"

class INGTApplication : public Implements< IUIApplication >
{
public:
	virtual ~INGTApplication() {}

    virtual void addListener( IApplicationListener & listener ) = 0;
    virtual void removeListener( IApplicationListener & listener ) = 0;
};

#endif//I_UI_APPLICATION_HPP
