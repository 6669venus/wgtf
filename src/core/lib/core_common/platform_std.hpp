#ifndef __PLATFORM_STD_HPP__
#define __PLATFORM_STD_HPP__

#if defined( _WIN32 )
#define NOEXCEPT
// TODO: Replace with thread_local in C++11 once Visual studio supports it
#define THREADLOCAL( type ) __declspec( thread ) type
#endif

#ifdef __APPLE__
#define NOEXCEPT noexcept
#define THREADLOCAL( type ) __thread type
#endif // __APPLE__

#endif // __PLATFORM_STD_HPP__
