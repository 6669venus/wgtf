#include "pch.hpp"
#include "type_converter.hpp"

#include <string>

namespace TypeConverter
{

PyObject * getData( std::wstring & data )
{
	PyObject * pRet = PyUnicode_FromWideChar( data.c_str(), data.size() );

	return pRet;
}

} // namespace TypeConverter

