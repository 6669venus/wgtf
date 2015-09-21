#pragma once
#ifndef TYPE_CONVERTER_HPP
#define TYPE_CONVERTER_HPP

namespace std
{
template<class _Elem,
	class _Traits,
	class _Alloc>
	class basic_string;
template<class _Elem>
	struct char_traits;
template<class _Ty>
	class allocator;
typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
	wstring;
} // namespace std


/**
 *	Collection of helper functions for converting between C++ and Python types.
 */
namespace TypeConverter
{
	PyObject * getData( const wchar_t * data );
	PyObject * getData( std::wstring & data );
} // namespace TypeConverter


#endif // TYPE_CONVERTER_HPP

