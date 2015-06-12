#include "generic_plugin.hpp"

//==============================================================================
PluginMain::PluginMain()
	: name_( NULL )
{
}


//==============================================================================
void PluginMain::init( const char * name )
{
	name_ = name;
}




static IMemoryAllocator *	s_Allocator = nullptr;
static IContextManager *	s_PluginContext = nullptr;

IContextManager * getPluginContext()
{
	if (s_PluginContext == nullptr)
	{
		TCHAR sharedMemoryName[] = SHARED_MEMORY_NAME;
		HANDLE sharedMemory = OpenFileMapping(
			FILE_MAP_READ,
			FALSE,
			sharedMemoryName );
		if (sharedMemory == nullptr)
		{
			return s_PluginContext;
		}
		s_PluginContext = *reinterpret_cast< IContextManager ** >(
			MapViewOfFile( sharedMemory,
			FILE_MAP_READ,
			0,
			0,
			sizeof( void * ) ) );
		assert( s_PluginContext != nullptr );
		UnmapViewOfFile( sharedMemory );
		CloseHandle( sharedMemory );
	}
	return s_PluginContext;
}


//==============================================================================
namespace Context{

	bool deregisterInterface( IInterface * pImpl )
	{
		IContextManager * pluginContext = getPluginContext();
		assert(pluginContext != nullptr );
		return pluginContext->deregisterInterface( pImpl );
	}

	void * queryInterface( const TypeId & name )
	{
		IContextManager * pluginContext = getPluginContext();
		assert(pluginContext != nullptr );
		return pluginContext->queryInterface( name );
	}

	void queryInterface( const TypeId & name, std::vector< void * > & o_Impls )
	{
		IContextManager * pluginContext = getPluginContext();
		assert(pluginContext != nullptr );
		return pluginContext->queryInterface( name, o_Impls );
	}

}/* Namespace context*/


//==============================================================================
IMemoryAllocator * getMemoryAllocator()
{
	IContextManager * pluginContext = getPluginContext();
	if (s_Allocator == nullptr)
	{
		if (pluginContext == nullptr)
		{
			return s_Allocator;
		}
		s_Allocator = pluginContext->queryInterface< IMemoryAllocator >();
	}
	return s_Allocator;
}


//==============================================================================
void * operator new( std::size_t size )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new( size );
}


//==============================================================================
void * operator new ( std::size_t size, const std::nothrow_t & throwable )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new( size, throwable );
}


//==============================================================================
void * operator new[]( std::size_t size )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return getMemoryAllocator()->mem_new_array( size );
}


//==============================================================================
void * operator new[]( std::size_t size, const std::nothrow_t & throwable )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new_array( size, throwable );
}


//==============================================================================
void operator delete( void* ptr )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete( ptr );
}


//==============================================================================
void operator delete( void* ptr, const std::nothrow_t & throwable )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete( ptr, throwable );
}


//==============================================================================
void operator delete[]( void* ptr )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete_array( ptr );
}


//==============================================================================
void operator delete[]( void* ptr, const std::nothrow_t & throwable )
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete_array( ptr, throwable );
}


static PluginMain * s_pluginMain = nullptr;

//==============================================================================
extern "C" __declspec(dllexport) BOOL WINAPI DllMain( HANDLE hModule, DWORD reason, LPVOID reserved )
{
	switch( reason )
	{
	case DLL_PROCESS_ATTACH:
		{
			getPluginContext();
			break;
		}
	};
	return true;
}


extern PluginMain * createPlugin( IContextManager & contextManager );

//==============================================================================
extern "C" __declspec(dllexport) bool __cdecl PLG_CALLBACK( GenericPluginLoadState loadState )
{
	auto contextManager = getPluginContext();
	assert( contextManager );
	switch (loadState)
	{
	case GenericPluginLoadState::Create:
		s_pluginMain = createPlugin( *contextManager );
		return true;
									
	case GenericPluginLoadState::PostLoad:
		return s_pluginMain->PostLoad( *contextManager );
									  
	case GenericPluginLoadState::Initialise:
		s_pluginMain->Initialise( *contextManager );
		return true;
										
	case GenericPluginLoadState::Finalise:
		return s_pluginMain->Finalise( *contextManager );
									  
	case GenericPluginLoadState::Unload:
		s_pluginMain->Unload( *contextManager );
		return true;
									
	case GenericPluginLoadState::Destroy:
		delete s_pluginMain;
		s_pluginMain = nullptr;
		return true;

	default:
		break;
	}
	return false;
}


