#ifndef NGT_WINDOWS_HPP_INCLUDED
#define NGT_WINDOWS_HPP_INCLUDED

#if defined( _WIN32 )
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#endif

#ifdef __APPLE__
	typedef void* HMODULE;
	#define MAX_PATH PATH_MAX

	void OutputDebugString(const char* s)
	{
		printf("%s", s);
	}
	void OutputDebugString(const wchar_t* s)
	{
		wprintf (L"%ls", s);
	}
	void OutputDebugStringA(const char* s)
	{
		printf("%s", s);
	}
#endif // __APPLE__

#endif // NGT_WINDOWS_HPP_INCLUDED
