#include "generic_plugin_manager.hpp"
#include "dependency_system/i_interface.hpp"
#include "interfaces/i_plugin_context_creator.hpp"

#include "default_context_manager.hpp"
#include "logging/logging.hpp"

#include <algorithm>
#include <shlwapi.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>


#define STR( X ) #X
#define PLUGIN_GET_PROC_ADDRESS( hPlugin, func ) \
	::GetProcAddress( hPlugin, STR( func ) )

namespace
{

	class PluginContextManager
		: public IPluginContextManager
		, public IContextManagerListener
	{
		typedef std::vector< IPluginContextCreator * > ContextCreatorCollection;
	public:
		//==========================================================================
		PluginContextManager()
			: 	globalContext_( new DefaultContextManager() )
		{
			globalContext_->registerListener( *this );
		}

		//==========================================================================
		~PluginContextManager()
		{
			for( auto & it = contexts_.begin(); it != contexts_.end(); ++it )
			{
				delete it->second;
			}
			globalContext_->deregisterListener( *this );
		}


		//==========================================================================
		IContextManager * createContext( const PluginId & id )
		{
			// Create context
			auto pluginContext = new DefaultContextManager( globalContext_.get() );

			// Insert in context list
			contexts_.insert( std::make_pair( id, pluginContext ) );

			// Create ContextCreators and register them
			for( auto & it = contextCreators_.begin(); it != contextCreators_.end(); ++it )
			{
				auto contextCreator = it->second;

				// Create context
				IInterface * pInterface = contextCreator->createContext( id.c_str() );
				const char * type = contextCreator->getType();

				// Register
				pluginContext->registerInterfaceImpl( type,
					pInterface,
					IContextManager::Reg_Local );
				childContexts_[ contextCreator ].push_back( pInterface );
			}
			return pluginContext;
		}


		//==========================================================================
		IContextManager * getContext( const PluginId & id ) const
		{
			auto findIt = contexts_.find( id );
			if (findIt != contexts_.end())
			{
				return findIt->second;
			}
			return NULL;
		}


		//==========================================================================
		IContextManager * getGlobalContext() const
		{
			return globalContext_.get();
		}

		//==========================================================================
		void destroyContext( const PluginId & id )
		{
			auto findIt = contexts_.find( id );
			if (findIt != contexts_.end())
			{
				delete findIt->second;
				contexts_.erase( findIt );
			}
		}


		//==========================================================================
		void onInterfaceRegistered( IPluginContextCreator * contextCreator )
		{
			// Add ContextCreator to list
			assert( contextCreators_.find( contextCreator->getType() ) ==
				contextCreators_.end() );
			contextCreators_.insert(
				std::make_pair( contextCreator->getType(), contextCreator ) );

			// Register interface for ContextCreator
			for (auto context : contexts_)
			{
				IInterface * child =
					context.second->registerInterfaceImpl(
						contextCreator->getType(),
						contextCreator->createContext( context.first.c_str() ),
						IContextManager::Reg_Local );
				childContexts_[ contextCreator ].push_back( child );
			}
		}


		//==========================================================================
		void onInterfaceDeregistered( IPluginContextCreator * contextCreator )
		{
			// Remove ContextCreator from list
			for (auto it = contextCreators_.begin();
				it != contextCreators_.end();
				++it)
			{
				if (contextCreator != it->second)
				{
					continue;
				}
				auto findIt = childContexts_.find( contextCreator );
				assert( findIt != childContexts_.end() );
				for( auto & child : findIt->second )
				{
					for( auto & contextIt = contexts_.begin(); contextIt != contexts_.end(); ++contextIt )
					{
						if (contextIt->second->deregisterInterface( child ))
						{
							break;
						}
					}
				}
				childContexts_.erase( findIt );
				contextCreators_.erase( it );
				return;
			}
		}

	private:
		typedef std::vector< IInterface * > InterfaceCollection;
		typedef std::map< IPluginContextCreator *, InterfaceCollection > ContextChildrenCollection;
		ContextChildrenCollection							childContexts_;
		std::map< PluginId, IContextManager * >				contexts_;
		std::map< std::string, IPluginContextCreator * >	contextCreators_;
		std::unique_ptr< IContextManager >					globalContext_;
	};
 

	std::wstring processPluginFilename( const std::wstring& filename )
	{
		// PathCanonicalize does not convert '/' to '\\'
		WCHAR normalisedPath[ MAX_PATH ];
		std::copy( filename.c_str(),
			filename.c_str() + filename.size(),
			normalisedPath );
		normalisedPath[ filename.size() ] = L'\0';
		std::replace( normalisedPath,
			normalisedPath + filename.size(),
			L'/',
			L'\\' );

		WCHAR temp[MAX_PATH];

		if (PathIsRelative( normalisedPath ))
		{
			WCHAR exePath[MAX_PATH];
			GetModuleFileName( NULL, exePath, MAX_PATH );
			PathRemoveFileSpec( exePath );
			PathAppend( exePath, normalisedPath );
			PathCanonicalize( temp, exePath );
		}
		else
		{
			PathCanonicalize( temp, normalisedPath );
		}


		PathRemoveExtension( temp );
#ifdef _DEBUG
		const size_t len = ::wcsnlen( temp, MAX_PATH );
		if (::wcsncmp( temp + len - 2, L"_d", 2 ) != 0)
		{
			wcscat_s( temp, L"_d" );
		}
#endif
		PathAddExtension( temp, L".dll" );

		return  temp;
	}


	//==============================================================================
	class NotifyPlugin
	{
	public:
		typedef bool ( *CallbackFunc )( GenericPluginLoadState loadState );

		NotifyPlugin(
			GenericPluginManager & pluginManager,
 			GenericPluginLoadState loadState  )
				: pluginManager_( pluginManager )
				, loadState_( loadState )
		{
		}

		bool operator()( HMODULE hPlugin )
		{
			CallbackFunc pCallback = GetPluginCallbackFunc( hPlugin );
			return pCallback ?
				pCallback( loadState_ ) :
				false;
		}

	private:
		static CallbackFunc GetPluginCallbackFunc( HMODULE hPlugin )
		{
			return (CallbackFunc) PLUGIN_GET_PROC_ADDRESS( hPlugin, PLG_CALLBACK );
		}

	protected:
		GenericPluginManager & pluginManager_;
		GenericPluginLoadState loadState_;
	};


	//==============================================================================
	class NotifyPluginPostLoad
		: public NotifyPlugin
	{
	public:
		NotifyPluginPostLoad(
			GenericPluginManager & pluginManager )
			: NotifyPlugin (pluginManager, GenericPluginLoadState::PostLoad )
		{
		}

		~NotifyPluginPostLoad()
		{
			pluginManager_.unloadPlugins( pluginsToUnload_ );
		}

		bool operator()( HMODULE hPlugin )
		{
			bool br = NotifyPlugin::operator()( hPlugin );
			if (!br)
			{
				pluginsToUnload_.push_back( hPlugin );
			}
			return br;
		}

	private:
		std::vector< HMODULE > pluginsToUnload_;
	};

}


//==============================================================================
GenericPluginManager::GenericPluginManager()
	: contextManager_( new PluginContextManager() )
{
	TCHAR * szName = SHARED_MEMORY_NAME;
	sharedMemory_ = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL, 
		PAGE_READWRITE,
		0,
		sizeof( void * ),
		szName );
	assert( sharedMemory_ != NULL );
}


//==============================================================================
GenericPluginManager::~GenericPluginManager()
{
	// uninitialise in the reverse order. yes, we need a copy here.
	PluginList plugins( plugins_.rbegin(), plugins_.rend() );
	unloadPlugins( plugins );
	CloseHandle( sharedMemory_ );
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
	LPVOID pBuf = MapViewOfFile(
		sharedMemory_,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof( void * ) );
	assert( pBuf != NULL );
	IContextManager * pluginContext = contextManager_->createContext( processedFileName );
	*reinterpret_cast< IContextManager ** >( pBuf ) = pluginContext;


	const size_t errorMsgLength = 4096;
	char errorMsg[ errorMsgLength ];
	bool hadError = false;
	DWORD lastError;

	// "This value must be identical to the value returned by a previous call
	// to the MapViewOfFile or MapViewOfFileEx function."
	const BOOL unmapSuccess = UnmapViewOfFile( pBuf );
	if (!unmapSuccess)
	{
		lastError = GetLastError();
		if (lastError != ERROR_SUCCESS)
		{
			DWORD result = FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM,
				0, lastError, 0, errorMsg, ( DWORD ) errorMsgLength, 0 );
			hadError = true;
		}

		NGT_ERROR_MSG( "Could not unmap view of file %S (from %S): %s\n",
			filename.c_str(),
			processedFileName.c_str(),
			hadError ? errorMsg : "Unknown error" );
		return HMODULE( nullptr );
	}

	HMODULE hPlugin = 
		::LoadLibraryW( processedFileName.c_str() );
	if (hPlugin != NULL)
	{
		plugins_.push_back( hPlugin );
	}
	else
	{
		contextManager_->destroyContext( processedFileName );

		hadError = false;
		lastError = GetLastError();
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
	IContextManager * contextManager =
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


#undef PLUGIN_GET_PROC_ADDRESS
#undef STR