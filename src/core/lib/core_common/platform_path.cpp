#include "platform_path.hpp"

void AddDllExtension(wchar_t* file)
{
#ifdef _WIN32
	PathAddExtension(file, L".dll");
#elif __APPLE__
	PathAddExtension(file, L".dylib");
#endif
}

#ifdef __APPLE__

#include <codecvt>
#include <cwchar>
#include <locale>
#include <assert.h>
#include <stdlib.h>

bool PathIsRelative(const char* path)
{
	assert(path);
	return *path == '.' || *path == '/';
}

bool PathIsRelative(const wchar_t* path)
{
	assert(path);
	return *path == L'.' || *path == L'/';
}

bool PathCanonicalizeW(wchar_t*  dst, const wchar_t* src)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
	char path[PATH_MAX];
	realpath( conv.to_bytes(src).c_str(), path );
	wcscpy(dst, conv.from_bytes(path).c_str());
	return true;
}

bool PathCanonicalize(wchar_t*  dst, const wchar_t* src)
{
	return PathCanonicalizeW(dst, src);
}

void PathRemoveExtension(wchar_t* path)
{
	wchar_t* wc = path;
	while (wchar_t* p =  wcschr(wc, L'.'))
		wc = p + 1;
	if (wc != path)
		*(--wc) = 0;
}

bool PathAddExtension(wchar_t* path, const wchar_t* ext)
{
	wcscat(path, ext);
	return true;
}

bool PathRemoveFileSpecA(char* path)
{
	char* c = path;
	while (char* p = strchr(c, '/'))
		c = p + 1;
	if (c != path)
		--c;
	*c = 0;
	return true;
}

bool PathRemoveFileSpecW(wchar_t* path)
{
	wchar_t* wc = path;
	while (wchar_t* p =  wcschr(wc, L'/'))
		wc = p + 1;
	if (wc != path)
		--wc;
	*wc = 0;
	return true;
}

bool PathRemoveFileSpec(char* path)
{
	return PathRemoveFileSpecA(path);
}

bool PathRemoveFileSpec(wchar_t* path)
{
	return PathRemoveFileSpecW(path);
}

bool PathAppendW(wchar_t* path, const wchar_t* more)
{
	wcscat(path, more);
	return true;
}

bool PathAppend(wchar_t* path, const wchar_t* more)
{
	return PathAppendW(path, more);
}

#endif // __APPLE__
