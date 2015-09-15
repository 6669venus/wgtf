#pragma once
#ifndef I_V0_PYTHON_INTERPRETER_V0_HPP
#define I_V0_PYTHON_INTERPRETER_V0_HPP

#include "core_dependency_system/i_interface.hpp"

DECLARE_INTERFACE_BEGIN( IPythonInterpreter, 0, 0 )
	virtual bool import( const char* name ) = 0;
DECLARE_INTERFACE_END()

#endif // I_V0_PYTHON_INTERPRETER_V0_HPP

