#include "python_meta_type.hpp"


bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return (lhs.name() == rhs.name());
}


bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs )
{
	return !operator==( lhs, rhs );
}


PythonMetaType::PythonMetaType()
{
}


PythonMetaType::PythonMetaType( const char * name )
	: name_( name )
{
}


PythonMetaType::~PythonMetaType()
{
}


const std::string & PythonMetaType::name() const
{
	return name_;
}

