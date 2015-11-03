#pragma once
#ifndef I_PYTHON_TYPE_HPP
#define I_PYTHON_TYPE_HPP


/**
 *	This class is for wrapping Python types inside a Variant.
 */
class IPythonMetaType
{
public:
	/**
	 *	Get the name of the Python type.
	 *	@return the name of the type or null if it's invalid.
	 */
	virtual const char * name() const = 0;
};


#endif // I_PYTHON_TYPE_HPP
