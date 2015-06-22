#include "generic_plugin.hpp"
#include "interfaces/i_memory_allocator.hpp"
#include "ngt_core_common/shared_library.hpp"
#include <windows.h>


namespace
{

	IContextManager * getPluginContext()
	{
		static IContextManager * s_pluginContext = nullptr;
		if (s_pluginContext == nullptr)
		{
			SharedLibrary main( nullptr );
			auto pluginContext = main.findSymbol< IContextManager * >( "s_pluginContext" );
			if (pluginContext)
			{
				s_pluginContext = *pluginContext;
			}
		}
		return s_pluginContext;
	}

	IMemoryAllocator * getMemoryAllocator()
	{
		static IMemoryAllocator * s_allocator = nullptr;
		if (s_allocator == nullptr)
		{
			IContextManager * pluginContext = getPluginContext();
			if (pluginContext == nullptr)
			{
				return s_allocator;
			}
			s_allocator = pluginContext->queryInterface< IMemoryAllocator >();
		}
		return s_allocator;
	}

	class StaticInitializer
	{
	public:
		StaticInitializer()
		{
			// Ensure plugin context is cached before plugin is completely loaded
			getPluginContext();
		}
	};

	static StaticInitializer s_staticInitializer;

}


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


//==============================================================================
namespace Context
{

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


PluginMain * createPlugin( IContextManager & contextManager );

//==============================================================================
EXPORT bool __cdecl PLG_CALLBACK( GenericPluginLoadState loadState )
{
	static PluginMain * s_pluginMain = nullptr;
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


