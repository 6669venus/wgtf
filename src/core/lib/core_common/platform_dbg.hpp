#ifndef __PLATFORM_DBG_HPP__
#define __PLATFORM_DBG_HPP__

#include <string>

#if defined( _WIN32 )
#include <windows.h>
#endif

namespace wgt
{
#ifdef __APPLE__

void OutputDebugString(const char* s);
void OutputDebugStringA(const char* s);
void OutputDebugString(const wchar_t* s);

#endif // __APPLE__

bool FormatLastErrorMessage(std::string& errorMsg);
} // end namespace wgt
#endif // __PLATFORM_DBG_HPP__
