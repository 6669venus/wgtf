#if _WIN32_WINNT < 0x0502
#undef _WIN32_WINNT
#undef NTDDI_VERSION
#define _WIN32_WINNT _WIN32_WINNT_WS03 //required for SetDllDirectory
#define NTDDI_VERSION NTDDI_WS03
#include <windows.h>
#endif

#include "generic_plugin_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "notify_plugin.hpp"
#include "plugin_context_manager.hpp"
#include "core_ngt_common/environment.hpp"

#include "core_logging/logging.hpp"

#include <algorithm>
#include <shlwapi.h>
#include <iterator>
#include <cstdint>

namespace
{

	static char ngtHome[MAX_PATH];
	static WCHAR exePath[MAX_PATH];

	const char NGT_HOME[] = "NGT_HOME";

	void setContext( IComponentContext* context )
	{
		const char ENV_VAR_NAME[] = "PLUGIN_CONTEXT_PTR";
		if (context)
		{
			auto ptr = reinterpret_cast< uintptr_t >( context );
			char buf[33] = {};
			size_t i = sizeof(buf) - 2;
			while (true)
			{
				char digit = ptr % 16;

				if (digit < 10)
				{
					buf[i] = '0' + digit;
				}
				else
				{
					buf[i] = 'a' + digit - 10;
				}

				ptr = ptr / 16;

				if (ptr == 0 || i == 0)
				{
					break;
				}

				--i;
			}

			Environment::setValue( ENV_VAR_NAME, buf + i );
		}
		else
		{
			Environment::unsetValue( ENV_VAR_NAME );
		}
	}
}


//==============================================================================
GenericPluginManager::GenericPluginManager()
	: contextManager_( new PluginContextManager() )
{
	if (!Environment::getValue<MAX_PATH>( NGT_HOME, ngtHome ))
	{
		GetModuleFileNameA( NULL, ngtHome, MAX_PATH );
		PathRemoveFileSpecA( ngtHome );
		Environment::setValue( NGT_HOME, ngtHome );
	}

	size_t convertedChars = 0;
	mbstowcs_s( &convertedChars, exePath, MAX_PATH, ngtHome, _TRUNCATE );
	assert( convertedChars );

	char path[2048];
	if(Environment::getValue<2048>( "PATH", path ))
	{
		std::string newPath( "\"" );
		newPath += ngtHome;
		newPath += "\";";
		newPath += path;
		Environment::setValue( "PATH", newPath.c_str() );
	}

	SetDllDirectoryA( ngtHome );
}


//==============================================================================
GenericPluginManager::~GenericPluginManager()
{
	// uninitialise in the reverse order. yes, we need a copy here.
	PluginList plugins( plugins_.rbegin(), plugins_.rend() );
	unloadPlugins( plugins );
}


//==============================================================================
void GenericPluginManager::loadPlugins( 
	const std::vector< std::wstring >& plugins )
{
	PluginList plgs;
	std::transform( 
		std::begin( plugins ),
		std::end( plugins ), 
		std::back_inserter( plgs ),
		std::bind( 
			&GenericPluginManager::loadPlugin, this, std::placeholders::_1) );

	notifyPlugins( plgs, 
		NotifyPlugin ( *this, GenericPluginLoadState::Create ) );

	notifyPlugins( plgs, NotifyPluginPostLoad( *this ) );

	notifyPlugins( plgs, 
		NotifyPlugin ( *this, GenericPluginLoadState::Initialise ) );
}


//==============================================================================
void GenericPluginManager::unloadPlugins(
	const std::vector< std::wstring >& plugins )
{
	PluginList plgs;
	for ( auto & filename : plugins )
	{
		HMODULE hPlugin = 
			::GetModuleHandleW( processPluginFilename( filename ).c_str() );
		if (hPlugin)
		{
			plgs.push_back( hPlugin );
		}
	}

	unloadPlugins( plgs );
}

using namespace std;

//==============================================================================
void GenericPluginManager::unloadPlugins( const PluginList& plugins )
{
	if (plugins.empty())
	{
		return;
	}

	for( int state = Finalise; state < Destroy; ++state)
	{
		notifyPlugins( plugins, 
			NotifyPlugin ( *this, ( GenericPluginLoadState ) state ) );
	}

	// Do in reverse order of load
	std::for_each( std::begin( plugins ), std::end( plugins ), std::bind( 
		&GenericPluginManager::unloadContext, this, std::placeholders::_1 ) );

	// Notify plugins of destroy - Matches Create notification
	notifyPlugins( plugins,  NotifyPlugin ( *this, Destroy ) );

	// Calls FreeLibrary - matches loadPlugin() LoadLibraryW
	std::for_each( std::begin( plugins ), std::end( plugins ), std::bind( 
		&GenericPluginManager::unloadPlugin, this, std::placeholders::_1 ) );

	auto it = memoryContext_.begin();
	for( ; it != memoryContext_.end(); ++it )
	{
		::OutputDebugString( it->first.c_str() );
		::OutputDebugString( L"\n" );
		delete it->second;
	}
	memoryContext_.clear();
}


//==============================================================================
void GenericPluginManager::notifyPlugins( 
	const PluginList& plugins, NotifyFunction func )
{
	std::for_each( std::begin( plugins ), std::end( plugins ), func );
}

//==============================================================================
HMODULE GenericPluginManager::loadPlugin( const std::wstring & filename )
{
	auto & processedFileName = processPluginFilename( filename );

	setContext( contextManager_->createContext( processedFileName ) );
	HMODULE hPlugin = ::LoadLibraryW( processedFileName.c_str() );
	setContext( nullptr );

	if (hPlugin != NULL)
	{
		plugins_.push_back( hPlugin );
	}
	else
	{
		contextManager_->destroyContext( processedFileName );

		const size_t errorMsgLength = 4096;
		char errorMsg[ errorMsgLength ];
		bool hadError = false;
		DWORD lastError = GetLastError();

		if (lastError != ERROR_SUCCESS)
		{
			DWORD result = FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM,
				0, lastError, 0, errorMsg, ( DWORD ) errorMsgLength, 0 );
			hadError = true;
		}
		NGT_ERROR_MSG( "Could not load plugin %S (from %S): %s\n",
			filename.c_str(),
			processedFileName.c_str(),
			hadError ? errorMsg : "Unknown error" );
	}
	return hPlugin;
}


//==============================================================================
void GenericPluginManager::unloadContext( HMODULE hPlugin )
{
	PluginList::iterator it = 
		std::find( std::begin( plugins_ ), std::end( plugins_ ), hPlugin );
	if ( it == std::end( plugins_ ) )
	{
		return;
	}

	wchar_t path[ MAX_PATH ];
	GetModuleFileName( *it, path, MAX_PATH );
	IComponentContext * contextManager =
		contextManager_->getContext( path );
	IMemoryAllocator * memoryAllocator =
		contextManager->queryInterface< IMemoryAllocator >();
	contextManager_->destroyContext( path );
	memoryContext_.insert( std::make_pair( path, memoryAllocator ) );
}

//==============================================================================
bool GenericPluginManager::unloadPlugin( HMODULE hPlugin )
{
	if (!hPlugin)
	{
		return false;
	}

	PluginList::iterator it = 
		std::find( std::begin( plugins_ ), std::end( plugins_ ), hPlugin );
	assert( it != std::end( plugins_ ) );

	// Get path before FreeLibrary
	wchar_t path[ MAX_PATH ];
	const DWORD pathLength = GetModuleFileName( *it, path, MAX_PATH );
	assert( pathLength > 0 );

	::FreeLibrary( hPlugin );
	plugins_.erase ( it );
	
	return true;
}


//==============================================================================
IPluginContextManager & GenericPluginManager::getContextManager() const
{
	return *contextManager_.get();
}


//==============================================================================
void * GenericPluginManager::queryInterface( const char * name ) const
{
	return getContextManager().getGlobalContext()->queryInterface(
		name );
}

//==============================================================================
std::wstring GenericPluginManager::processPluginFilename(const std::wstring& filename)
{
	// PathCanonicalize does not convert '/' to '\\'
	WCHAR normalisedPath[MAX_PATH];
	std::copy(filename.c_str(), filename.c_str() + filename.size(), normalisedPath);
	normalisedPath[filename.size()] = L'\0';
	std::replace(normalisedPath, normalisedPath + filename.size(), L'/', L'\\');

	WCHAR temp[MAX_PATH];

	if (PathIsRelative(normalisedPath))
	{
		WCHAR exePath[MAX_PATH];
		if (contextManager_->getExecutablePath())
		{
			mbstowcs(exePath, contextManager_->getExecutablePath(), strlen(contextManager_->getExecutablePath()) + 1);
		}
		else
		{
			GetModuleFileName(NULL, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
		}

		PathAppend(exePath, normalisedPath);
		PathCanonicalize(temp, exePath);
	}
	else
	{
		PathCanonicalize(temp, normalisedPath);
	}


	PathRemoveExtension(temp);

#ifdef _DEBUG
	const size_t len = ::wcsnlen(temp, MAX_PATH);
	if (::wcsncmp(temp + len - 2, L"_d", 2) != 0)
	{
		wcscat_s(temp, L"_d");
	}
#endif
	PathAddExtension(temp, L".dll");

	return  temp;
}
