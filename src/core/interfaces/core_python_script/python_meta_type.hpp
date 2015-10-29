#pragma once
#ifndef I_PYTHON_TYPE_HPP
#define I_PYTHON_TYPE_HPP


#include <string>


class PythonMetaType;
bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs );
bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs );


/**
 *	This class is for wrapping Python types inside a Variant.
 */
class PythonMetaType
{
public:
	/**
	 *	Constructs an invalid type.
	 */
	PythonMetaType();

	/**
	 *	Constructs a representation of the Python type.
	 *	@param name name of a built-in type or name of a class.
	 */
	PythonMetaType( const char * name );

	~PythonMetaType();

	const std::string & name() const;

private:
	std::string name_;
};


#endif // I_PYTHON_TYPE_HPP
