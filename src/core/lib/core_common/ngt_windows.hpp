#ifndef NGT_WINDOWS_HPP_INCLUDED
#define NGT_WINDOWS_HPP_INCLUDED

#if defined( _WIN32 )
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
	#include <objbase.h>
	#include <DbgHelp.h>
	#define NOEXCEPT
	typedef unsigned __int64 __uint64;
#endif

#ifdef __APPLE__
#include <stddef.h>
#include <inttypes.h>

#define MAX_PATH PATH_MAX
#define _MAX_PATH PATH_MAX
#define WCHAR wchar_t
#define _TRUNCATE 0
#define WINAPI
#define _Inout_
#define _In_opt_
#define _Out_
#define _In_
#define INVALID_HANDLE_VALUE 0
#define ERROR_SUCCESS 0
#define FORMAT_MESSAGE_FROM_SYSTEM 0
#define __stdcall
#define TRUE true
#define restrict
#define GMEM_MOVEABLE 0x0002
#define CF_TEXT 1
#define NOEXCEPT noexcept

#define SYMOPT_LOAD_LINES 0x1
#define SYMOPT_DEFERRED_LOADS 0x2
#define SYMOPT_UNDNAME 0x4

typedef void* HMODULE;
typedef void* HANDLE;
typedef HANDLE HGLOBAL;
typedef HANDLE HWND;
typedef void* PVOID;
typedef char* LPTSTR;
typedef const char* LPCTSTR;
typedef const char* LPCSTR;
typedef char* PSTR;
typedef const char* PCSTR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef DWORD* PDWORD;
typedef bool BOOL;
typedef int64_t __int64;
typedef uint64_t __uint64;
typedef __uint64 DWORD64, *PDWORD64;
typedef __uint64 ULONG64, *PULONG64;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef long LONG;
typedef unsigned int UINT;
typedef ULONG* PULONG;
typedef DWORD SIZE_T;
typedef void *LPVOID;
typedef const void *LPCVOID;
typedef LONG HRESULT;

#define FAILED(hr) (((HRESULT)(hr)) < 0)

typedef struct _GUID {
  DWORD Data1;
  WORD  Data2;
  WORD  Data3;
  BYTE  Data4[8];
} GUID;

typedef struct _SYMBOL_INFO
{
	ULONG SizeOfStruct;
	ULONG MaxNameLen;
	char Name[1];
	ULONG NameLen;

} SYMBOL_INFO, *PSYMBOL_INFO;

typedef struct _IMAGEHLP_LINE64
{
	ULONG SizeOfStruct;
	const char* FileName;
	int LineNumber;
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;

struct WIN32_FIND_DATA
{
	const wchar_t* cFileName;
};

typedef struct _MEMORY_BASIC_INFORMATION {
  PVOID  BaseAddress;
  PVOID  AllocationBase;
  DWORD  AllocationProtect;
  DWORD RegionSize;
  DWORD  State;
  DWORD  Protect;
  DWORD  Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

DWORD WINAPI GetModuleFileName(
  _In_opt_ HMODULE hModule,
  _Out_    wchar_t*  lpFilename,
  _In_     DWORD   nSize
)
{
	return 0;
}

DWORD WINAPI GetModuleFileNameA(
  _In_opt_ HMODULE hModule,
  _Out_    LPTSTR  lpFilename,
  _In_     DWORD   nSize
)
{
	return 0;
}

DWORD WINAPI GetModuleFileNameW(
  _In_opt_ HMODULE hModule,
  _Out_    const wchar_t*  lpFilename,
  _In_     DWORD   nSize
)
{
	return 0;
}

void* WINAPI GetProcAddress(
  _In_ HMODULE hModule,
  _In_ LPCSTR  lpProcName
)
{
	return 0;
}

void ZeroMemory(
  PVOID  Destination,
  DWORD Length
)
{}

int mbstowcs_s(
   size_t *pReturnValue,
   wchar_t *wcstr,
   size_t sizeInWords,
   const char *mbstr,
   size_t count
)
{
	return 0;
}

int strncpy_s(char *restrict dest, DWORD destsz,
                  const char *restrict src, DWORD count)
{
	return 0;
}

int sprintf_s(
   char *buffer,
   size_t sizeOfBuffer,
   const char *format,
   ...
)
{
	return 0;
}

void SetDllDirectoryA(const char* d)
{
}

BOOL PathRemoveFileSpecA(
  _Inout_ LPTSTR pszPath
)
{
	return true;
}

BOOL PathRemoveFileSpecW(
  _Inout_ const wchar_t* pszPath
)
{
	return true;
}

HMODULE WINAPI LoadLibraryW(
  _In_ const wchar_t* lpFileName
)
{
	return nullptr;
}

HMODULE WINAPI LoadLibraryA(
  _In_ const char* lpFileName
)
{
	return nullptr;
}

BOOL WINAPI FreeLibrary(
  _In_ HMODULE hModule
)
{
	return true;
}

HANDLE WINAPI GetCurrentProcess(void)
{
	return nullptr;
}

HMODULE WINAPI GetModuleHandleW(
  _In_opt_ const wchar_t* lpModuleName
)
{
	return nullptr;
}

SIZE_T WINAPI VirtualQuery(
  _In_opt_ LPCVOID                   lpAddress,
  _Out_    PMEMORY_BASIC_INFORMATION lpBuffer,
  _In_     SIZE_T                    dwLength
);

void OutputDebugString(const char* s)
{
}

void OutputDebugStringA(const char* s)
{
}

void OutputDebugString(const wchar_t* s)
{
}

BOOL PathRemoveFileSpec(
  _Inout_ LPTSTR pszPath
)
{
	return true;
}

BOOL PathRemoveFileSpec(
  const wchar_t* pszPath
)
{
	return true;
}

void PathRemoveExtension(
  _Inout_ wchar_t* pszPath
);

BOOL PathIsRelative(
  _In_ LPCTSTR lpszPath
)
{
	return true;
}

BOOL PathIsRelative(
  const wchar_t* lpszPath
)
{
	return true;
}

HANDLE WINAPI FindFirstFileW(
  _In_  const wchar_t*    lpFileName,
  _Out_ WIN32_FIND_DATA* lpFindFileData
)
{
	return nullptr;
}

HANDLE WINAPI FindNextFile(
  HANDLE    lpFileName,
  _Out_ WIN32_FIND_DATA* lpFindFileData
)
{
	return nullptr;
}

BOOL PathCanonicalize(
  _Out_ wchar_t*  lpszDst,
  _In_  const wchar_t* lpszSrc
);

BOOL PathCanonicalizeW(
  _Out_ wchar_t*  lpszDst,
  _In_  const wchar_t* lpszSrc
)
{
	return true;
}

BOOL PathAddExtension(
  _Inout_  wchar_t*  pszPath,
  _In_opt_ const wchar_t* pszExtension
);

BOOL PathAppend(
  _Inout_ wchar_t*  pszPath,
  _In_    const wchar_t* pszMore
)
{
	return true;
}

DWORD GetLastError()
{
	return 0;
}

#include <cstdarg>
DWORD WINAPI FormatMessageA(
  _In_     DWORD   dwFlags,
  _In_opt_ const void* lpSource,
  _In_     DWORD   dwMessageId,
  _In_     DWORD   dwLanguageId,
  _Out_    LPTSTR  lpBuffer,
  _In_     DWORD   nSize,
  _In_opt_ va_list *Arguments
);

BOOL WINAPI OpenClipboard(
  _In_opt_ HWND hWndNewOwner
)
{
	return true;
}

BOOL WINAPI EmptyClipboard(void)
{
	return true;
}

HGLOBAL WINAPI GlobalAlloc(
  _In_ UINT   uFlags,
  _In_ SIZE_T dwBytes
)
{
	return nullptr;
}

BOOL WINAPI CloseClipboard(void)
{
	return true;
}

LPVOID WINAPI GlobalLock(
  _In_ HGLOBAL hMem
)
{
	return nullptr;
}

BOOL WINAPI GlobalUnlock(
  _In_ HGLOBAL hMem
)
{
	return true;
}

HANDLE WINAPI SetClipboardData(
  _In_     UINT   uFormat,
  _In_opt_ HANDLE hMem
)
{
	return nullptr;
}

HGLOBAL WINAPI GlobalFree(
  _In_ HGLOBAL hMem
)
{
	return nullptr;
}

HANDLE WINAPI GetClipboardData(
  _In_ UINT uFormat
)
{
	return nullptr;
}

SIZE_T WINAPI GlobalSize(
  _In_ HGLOBAL hMem
)
{
	return 0u;
}

BOOL WINAPI IsClipboardFormatAvailable(
  _In_ UINT format
)
{
	return true;
}

HRESULT CoCreateGuid(
  _Out_ GUID *pguid
)
{
	return 0;
}

DWORD WINAPI GetCurrentDirectory(
  _In_  DWORD  nBufferLength,
  _Out_ wchar_t* lpBuffer
)
{
	return 0;
}

BOOL WINAPI SetCurrentDirectory(
  _In_ const wchar_t* lpPathName
)
{
	return true;
}

#endif // __APPLE__

#endif
