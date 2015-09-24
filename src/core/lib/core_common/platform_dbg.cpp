#include "platform_dbg.hpp"

#if defined( _WIN32 )

bool FormatLastErrorMessage(std::string& errorMsg)
{
	static const size_t errorMsgLength = 4096;
	errorMsg.resize(errorMsgLength);

	bool hadError = false;
	unsigned int lastError = GetLastError();

	if (lastError != ERROR_SUCCESS)
	{
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, lastError, 0, &errorMsg.front(), (unsigned int ) errorMsgLength, 0);
		hadError = true;
	}
	return hadError;
}

#endif

#ifdef __APPLE__
#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <dlfcn.h>
#include <string.h>

bool FormatLastErrorMessage(std::string& errorMsg)
{
	const char* dlerr = dlerror();

	if (dlerr)
	{
		errorMsg = dlerr;
	}
	else if ( errno )
	{
		errorMsg = strerror( errno );
	}

	return !errorMsg.empty();
}

void OutputDebugString(const char* s)
{
	printf("%s", s);
}

void OutputDebugString(const wchar_t* s)
{
	wprintf(L"%ls", s);
}

void OutputDebugStringA(const char* s)
{
	printf("%s", s);
}

#endif // __APPLE__