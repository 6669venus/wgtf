#include "ngt_windows.hpp"

#ifdef __APPLE__

DWORD WINAPI GetModuleFileName(_In_opt_ HMODULE hModule, _Out_ wchar_t* lpFilename, _In_ DWORD nSize)
{
	return 0;
}

DWORD WINAPI GetModuleFileNameA(_In_opt_ HMODULE hModule, _Out_ LPTSTR lpFilename, _In_ DWORD nSize)
{
	return 0;
}

DWORD WINAPI GetModuleFileNameW(_In_opt_ HMODULE hModule, _Out_ const wchar_t* lpFilename, _In_ DWORD nSize)
{
	return 0;
}

void* WINAPI GetProcAddress(_In_ HMODULE hModule, _In_ LPCSTR lpProcName)
{
	return 0;
}

void ZeroMemory(PVOID Destination, DWORD Length)
{

}

int mbstowcs_s(size_t *pReturnValue, wchar_t *wcstr, size_t sizeInWords, const char *mbstr, size_t count)
{
	return 0;
}

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
	return 0;
}

void SetDllDirectoryA(const char* d)
{

}

BOOL PathRemoveFileSpecW(_Inout_ const wchar_t* pszPath)
{
	return true;
}

HMODULE WINAPI LoadLibraryW(_In_ const wchar_t* lpFileName)
{
	return nullptr;
}

void OutputDebugString(const char* s)
{

}

void OutputDebugString(const wchar_t* s)
{

}

void OutputDebugStringA(const char* s)
{

}

BOOL PathRemoveFileSpec(_Inout_ LPTSTR pszPath)
{
	return true;
}

BOOL PathRemoveFileSpec(const wchar_t* pszPath)
{
	return true;
}

BOOL PathIsRelative(_In_ LPCTSTR lpszPath)
{
	return true;
}

BOOL PathIsRelative(const wchar_t* lpszPath)
{
	return true;
}

HANDLE WINAPI FindFirstFileW(_In_ const wchar_t* lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData)
{
	return nullptr;
}

HANDLE WINAPI FindNextFile(HANDLE lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData)
{
	return nullptr;
}

BOOL PathCanonicalizeW(_Out_ wchar_t* lpszDst, _In_ const wchar_t* lpszSrc)
{
	return true;
}

BOOL PathAppend(_Inout_ wchar_t* pszPath, _In_ const wchar_t* pszMore)
{
	return true;
}

DWORD GetLastError()
{
	return 0;
}

BOOL WINAPI OpenClipboard(_In_opt_ HWND hWndNewOwner)
{
	return true;
}

BOOL WINAPI EmptyClipboard(void)
{
	return true;
}

HGLOBAL WINAPI GlobalAlloc(_In_ UINT uFlags, _In_ SIZE_T dwBytes)
{
	return nullptr;
}

BOOL WINAPI CloseClipboard(void)
{
	return true;
}

LPVOID WINAPI GlobalLock(_In_ HGLOBAL hMem)
{
	return nullptr;
}

BOOL WINAPI GlobalUnlock(_In_ HGLOBAL hMem)
{
	return true;
}

HANDLE WINAPI SetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem)
{
	return nullptr;
}

HGLOBAL WINAPI GlobalFree(_In_ HGLOBAL hMem)
{
	return nullptr;
}

HANDLE WINAPI GetClipboardData(_In_ UINT uFormat)
{
	return nullptr;
}

SIZE_T WINAPI GlobalSize(_In_ HGLOBAL hMem)
{
	return 0u;
}

BOOL WINAPI IsClipboardFormatAvailable(_In_ UINT format)
{
	return true;
}

HRESULT CoCreateGuid(_Out_ GUID *pguid)
{
	return 0;
}

DWORD WINAPI GetCurrentDirectory(_In_ DWORD nBufferLength, _Out_ wchar_t* lpBuffer)
{
	return 0;
}

BOOL WINAPI SetCurrentDirectory(_In_ const wchar_t* lpPathName)
{
	return true;
}

LPWSTR WINAPI GetCommandLineW(void)
{
	return nullptr;
}

LPWSTR* CommandLineToArgvW(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumArgs)
{
	return nullptr;
}

BOOL PathAppendW(_Inout_ LPWSTR pszPath, _In_ LPCWSTR pszMore)
{
	return false;
}

DWORD WINAPI GetEnvironmentVariableA(_In_opt_ LPCTSTR lpName, _Out_opt_ LPTSTR lpBuffer, _In_ DWORD nSize)
{
	return 0;
}

BOOL WINAPI SetEnvironmentVariableA(_In_ LPCTSTR lpName, _In_opt_ LPCTSTR lpValue)
{
	return false;
}

#endif // __APPLE__