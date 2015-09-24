#pragma once
#ifndef I_PYTHON_SCRIPTING_ENGINE_HPP
#define I_PYTHON_SCRIPTING_ENGINE_HPP

#include "v0/i_scripting_engine_v0.hpp"

/**
 *	Interface to for using Python scripts with C++.
 *	Controls initialization and finalization of the Python interpreter.
 *	Interface for calling into the Python interpreter.
 *	Can have different implementations for each version of Python.
 */
DECLARE_LATEST_VERSION( IPythonScriptingEngine, 0, 0 )

#endif // I_PYTHON_SCRIPTING_ENGINE_HPP
