#pragma once

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"


class IComponentContext;


/**
 *	Obtains a Python scripting engine interface and runs unit tests on
 *	its interface.
 */
class MainApplication
	: public Implements< IApplication >
{
public:
	MainApplication( IComponentContext & contextManager );
	int startApplication() override;
	void quitApplication() override;

private:
	IComponentContext& contextManager_;
};
