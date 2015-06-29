#include "environment.hpp"
#include "ngt_windows.hpp"


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


