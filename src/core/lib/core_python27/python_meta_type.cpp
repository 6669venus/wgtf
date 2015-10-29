#include "pch.hpp"
#include "python_meta_type.hpp"


PythonMetaType::PythonMetaType()
{
}


PythonMetaType::PythonMetaType( const char * name )
	: name_( name )
{
}


bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return (lhs.name_ == rhs.name_);
}


bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return !operator==( lhs, rhs );
}

