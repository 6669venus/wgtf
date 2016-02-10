#pragma once
#ifndef I_V0_PYTHON_SCRIPTING_ENGINE_V1_HPP
#define I_V0_PYTHON_SCRIPTING_ENGINE_V1_HPP

#include "core_dependency_system/i_interface.hpp"

#include <memory>

class ObjectHandle;

DECLARE_INTERFACE_BEGIN( IPythonScriptingEngine, 0, 0 )

	/**
	 *	Add a path the the "sys.path" list to search when importing modules.
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@param path to be added to the "sys.path" list.
	 *		Does not check if the path is valid.
	 *		Calling code should check if the path is valid before appending.
	 *	@return false if path was unable to be appended to the "sys.path" list.
	 */
	virtual bool appendPath( const wchar_t * path ) = 0;


	/**
	 *	Import a Python module using the search paths in "sys.path".
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@param name the name of the module to import.
	 *		e.g. import( "test" ) will search for "test.py".
	 *	
	 *	@return module that has been imported or nullptr on failure.
	 */
	virtual ObjectHandle import( const char * name ) = 0;

	
	/**
	 *	Do both appendPath() and import().
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@param path to be added to the "sys.path" list.
	 *		Does not check if the path is valid.
	 *		Calling code should check if the path is valid before appending.
	 *	@param name the name of the module to import.
	 *		e.g. import( "test" ) will search for "test.py".
	 *	
	 *	@return module that has been imported or nullptr on failure.
	 */
	virtual ObjectHandle appendPathAndImport( const wchar_t * path,
		const char * moduleName ) = 0;


	/**
	 *	Check for any errors during script execution and print them.
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@return true if errors occurred.
	 */
	virtual bool checkErrors() = 0;

DECLARE_INTERFACE_END()

#endif // I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP

