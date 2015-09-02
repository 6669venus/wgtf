#ifndef __PLATFORM_DLL_HPP__
#define __PLATFORM_DLL_HPP__

#if defined( _WIN32 )
#include <windows.h>
#endif

#ifdef __APPLE__
#include <dlfcn.h>

typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HANDLE;
typedef HANDLE HGLOBAL;
typedef HANDLE HWND;

void* GetProcAddress(void* handle, const char* symbol);

#endif // __APPLE__

#endif // __PLATFORM_DLL_HPP__
