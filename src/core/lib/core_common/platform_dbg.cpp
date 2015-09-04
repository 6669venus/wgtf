#include "platform_dbg.hpp"

#if defined( _WIN32 )

bool FormatLastErrorMessage(std::string& errorMsg)
{
	static const size_t errorMsgLength = 4096;
	errorMsg.resize(errorMsgLength);

	bool hadError = false;
	uint lastError = GetLastError();

	if (lastError != ERROR_SUCCESS)
	{
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, lastError, 0, &errorMsg.front(), (uint) errorMsgLength, 0);
		hadError = true;
	}
	return hadError;
}

#endif

#ifdef __APPLE__
#include <stdio.h>

bool FormatLastErrorMessage(std::string& errorMsg)
{
	errorMsg = "Unknown error";
	return true;
}

void OutputDebugString(const char* s)
{
	printf("%s", s);
}

void OutputDebugString(const wchar_t* s)
{
	printf("%s", s);
}

void OutputDebugStringA(const char* s)
{
	printf("%s", s);
}

#endif // __APPLE__
