#pragma once
#ifndef I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP
#define I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_python_script/i_module.hpp"

#include <memory>


#include "i_type_converter.hpp"


DECLARE_INTERFACE_BEGIN( IPythonScriptingEngine, 0, 0 )

	/**
	 *	Add a path the the "sys.path" list to search when importing modules.
	 *	
	 *	@pre interpreter must be initialised.
	 *	
	 *	@param path to add.
	 */
	virtual bool appendPath( const wchar_t * path ) = 0;


	/**
	 *	Import a Python module using the search paths in "sys.path".
	 *	
	 *	@pre interpreter must be initialised.
	 *	
	 *	@param name the name of the module to import.
	 *		e.g. import( "test" ) will search for "test.py".
	 *	
	 *	@return module that has been imported or nullptr on failure.
	 */
	virtual std::shared_ptr< IPythonModule > import( const char * name ) = 0;


	/**
	 *	Register a new type converter.
	 *	
	 *	During type conversion, type converters are searched from
	 *	most-recently-added to first-added.
	 *	
	 *	@param converter the converter to be added.
	 */
	virtual void registerTypeConverter( IPythonTypeConverter & converter ) = 0;


	/**
	 *	Deregister a type converter that was previously registered.
	 *	
	 *	If the given converter cannot be found, it does nothing.
	 *	
	 *	@param converter to be removed.
	 */
	virtual void deregisterTypeConverter( IPythonTypeConverter & converter ) = 0;

DECLARE_INTERFACE_END()

#endif // I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP

