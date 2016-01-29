#ifndef __PLATFORM_PATH_HPP__
#define __PLATFORM_PATH_HPP__

void AddDllExtension(wchar_t* file);

#if defined( _WIN32 )
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#endif

#ifdef __APPLE__

#define MAX_PATH PATH_MAX
#define _MAX_PATH PATH_MAX

bool PathIsRelative(const char* path);
bool PathIsRelative(const wchar_t* path);

bool PathCanonicalize(wchar_t*  dst, const wchar_t* src);
bool PathCanonicalizeW(wchar_t*  dst, const wchar_t* src);

void PathRemoveExtension(wchar_t* path);
bool PathAddExtension(wchar_t* path, const wchar_t* ext);

bool PathRemoveFileSpecA(char* path);
bool PathRemoveFileSpecW(wchar_t* path);
bool PathRemoveFileSpec(char* path);
bool PathRemoveFileSpec(wchar_t* path);

bool PathAppend(wchar_t* path, const wchar_t* more);
bool PathAppendW(wchar_t* path, const wchar_t* more);

void PathFileName(wchar_t* file, const wchar_t* path);

#endif // __APPLE__

#endif // __PLATFORM_PATH_HPP__
