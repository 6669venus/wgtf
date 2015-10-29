#pragma once
#ifndef I_PYTHON_TYPE_HPP
#define I_PYTHON_TYPE_HPP

#include <string>

class PythonMetaType
{
public:
	PythonMetaType();
	PythonMetaType( const char * name );
	std::string name_;
};


bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs );
bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs );


#endif // I_PYTHON_TYPE_HPP
