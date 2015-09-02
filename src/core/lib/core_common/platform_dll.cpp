#include "platform_dll.hpp"

void* GetProcAddress(void* handle, const char* symbol)
{
	return dlsym(handle, symbol);
}
