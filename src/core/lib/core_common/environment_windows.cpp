#include "environment.hpp"
//#include "ngt_windows.hpp"

#ifdef __APPLE__

#define FALSE false

typedef unsigned int DWORD;
typedef bool BOOL;

unsigned int GetEnvironmentVariableA(
  const char* lpName,
  char*  lpBuffer,
  unsigned int   nSize
)
{
	return 0;
}

bool SetEnvironmentVariableA(
  const char* lpName,
  const char* lpValue
)
{
	return false;
}

#endif // __APPLE__

bool Environment::getValue( const char* name, char* value, size_t valueSize )
{
	if (!name || !value)
	{
		return false;
	}

	auto len = GetEnvironmentVariableA( name, value, static_cast< DWORD >( valueSize ) );

	return len > 0 && len < valueSize;
}


bool Environment::setValue( const char* name, const char* value )
{
	if (!name || !value)
	{
		return false;
	}

	auto ret = SetEnvironmentVariableA( name, value );

	return ret != FALSE;
}


bool Environment::unsetValue( const char* name )
{
	if (!name)
	{
		return false;
	}

	auto ret = SetEnvironmentVariableA( name, NULL );

	return ret != FALSE;
}
