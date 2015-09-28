#pragma once
#ifndef I_V0_PYTHON_MODULE_V0_HPP
#define I_V0_PYTHON_MODULE_V0_HPP


/**
 *	Interface to using a Python module that has been imported.
 *	TODO interface versioning.
 */
class IPythonModule
{
public:
	/**
	 *	@return true if the module was imported successfully.
	 */
	virtual bool exists() const = 0;

	/**
	 *	Call a method in the module.
	 *	
	 *	@pre module must have been imported.
	 *	
	 *	@param methodName name of the method to be called.
	 *	
	 *	@return TODO return value/error handling.
	 */
	virtual bool callMethod( const char * methodName ) const = 0;
};

#endif // I_V0_PYTHON_MODULE_V0_HPP
