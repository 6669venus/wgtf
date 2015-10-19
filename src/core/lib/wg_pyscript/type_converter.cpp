#include "pch.hpp"
#include "type_converter.hpp"

#include <string>

namespace TypeConverter
{

/**
 *	@pre data must be null-terminated.
 */
PyObject * getData( const wchar_t * data )
{
	PyObject * pRet = PyUnicode_FromWideChar( data, wcslen( data ) );

	return pRet;
}


PyObject * getData( std::wstring & data )
{
	PyObject * pRet = PyUnicode_FromWideChar( data.c_str(), data.size() );

	return pRet;
}

} // namespace TypeConverter

