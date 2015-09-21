#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"


/**
 *	Interface to Python 2.7.x.
 */
class Python27ScriptingEngine
	: public Implements< IPythonScriptingEngine > // Always implement latest version
{
public:
	/**
	 *	The Python interpreter must be initialized before it can be used.
	 *	@return true on success.
	 */
	bool init();


	/**
	 *	Free all resources used by Python before execution ends.
	 *	If the interpreter was not initialized, it should do nothing.
	 */
	void fini();


	/**
	 *	Import a Python module using the search paths in "sys.path".
	 *	
	 *	@pre interpreter must be initialised.
	 *	
	 *	@param name the name of the module to import.
	 *		e.g. import( "test" ) will search for "test.py".
	 *	
	 *	@return true on success.
	 */
	bool import( const char* name ) override;
};


#endif // PYTHON_SCRIPTING_ENGINE_HPP

