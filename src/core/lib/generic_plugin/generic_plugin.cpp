#include "generic_plugin.hpp"
#include "interfaces/i_memory_allocator.hpp"
#include "ngt_core_common/shared_library.hpp"
#include "ngt_core_common/environment.hpp"
#include "ngt_core_common/ngt_windows.hpp"
#include <cstdint>


namespace
{

	IComponentContext * getContext()
	{
		static IComponentContext * s_context = nullptr;
		if (s_context == nullptr)
		{
			char buf[33] = {};
			if (Environment::getValue( "PLUGIN_CONTEXT_PTR", buf ))
			{
				// convert hex string to pointer value
				uintptr_t ptr = 0;
				for (const char* pc = buf; *pc; ++pc )
				{
					// to lower case
					char c = *pc | 0x20;
					uintptr_t digit = 0;
					if (c >= '0' && c <= '9')
					{
						digit = c - '0';
					}
					else if (c >= 'a' && c <= 'f')
					{
						digit = c - 'a' + 10;
					}
					else
					{
						break;
					}

					ptr = ptr * 16 + digit;
				}

				s_context = reinterpret_cast< IComponentContext* >( ptr );
			}
		}
		return s_context;
	}

	IMemoryAllocator * getMemoryAllocator()
	{
		static IMemoryAllocator * s_allocator = nullptr;
		if (s_allocator == nullptr)
		{
			IComponentContext * context = getContext();
			if (context == nullptr)
			{
				return s_allocator;
			}
			s_allocator = context->queryInterface< IMemoryAllocator >();
		}
		return s_allocator;
	}

	class StaticInitializer
	{
	public:
		StaticInitializer()
		{
			// Ensure plugin context is cached before plugin is completely loaded
			getContext();
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
		IComponentContext * context = getContext();
		assert(context != nullptr );
		return context->deregisterInterface( pImpl );
	}

	void * queryInterface( const TypeId & name )
	{
		IComponentContext * context = getContext();
		assert(context != nullptr );
		return context->queryInterface( name );
	}

	void queryInterface( const TypeId & name, std::vector< void * > & o_Impls )
	{
		IComponentContext * context = getContext();
		assert(context != nullptr );
		return context->queryInterface( name, o_Impls );
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
void * operator new ( std::size_t size, const std::nothrow_t & throwable ) NOEXCEPT
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
void * operator new[]( std::size_t size, const std::nothrow_t & throwable ) NOEXCEPT
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new_array( size, throwable );
}


//==============================================================================
void operator delete( void* ptr ) NOEXCEPT
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
void operator delete( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
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
void operator delete[]( void* ptr ) NOEXCEPT
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
void operator delete[]( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
{
	IMemoryAllocator * memAlloc = getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete_array( ptr, throwable );
}


PluginMain * createPlugin( IComponentContext & contextManager );

//==============================================================================
EXPORT bool __cdecl PLG_CALLBACK( GenericPluginLoadState loadState )
{
	static PluginMain * s_pluginMain = nullptr;
	auto contextManager = getContext();
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


