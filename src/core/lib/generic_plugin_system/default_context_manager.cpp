#include "default_context_manager.hpp"
#include "interfaces/i_plugin_context_creator.hpp"
#include <assert.h>
#include <unordered_map>

namespace
{
	static const TypeId s_PluginContextCreatorTypeId =
		TypeId::getType< IPluginContextCreator >();
}

class RTTIHelper
{
public:
	//==========================================================================
	RTTIHelper( IInterface * pImpl )
		: pImpl_( pImpl )
	{
	}

	//==========================================================================
	~RTTIHelper()
	{
		delete pImpl_;
	}

	//==========================================================================
	void * queryInterface(
		const TypeId & name )
	{
		auto & findIt = typeCache_.find( name );
		if (findIt != typeCache_.end())
		{
			return findIt->second;
		}
		void * found = pImpl_->queryInterface( name );
		if (found)
		{
			typeCache_.insert( std::make_pair( name, found ) );
			return found;
		}
		return nullptr;
	}

	IInterface * getImpl() const { return pImpl_; }
	bool owns() const { return owns_; }

private:
	IInterface *								pImpl_;
	bool										owns_;
	std::unordered_map< const TypeId, void * >	typeCache_;
};


//==============================================================================
DefaultContextManager::DefaultContextManager( IContextManager * parentContext )
	: parentContext_( parentContext )
{
}


//==============================================================================
DefaultContextManager::~DefaultContextManager()
{
	for( auto & interface : interfaces_ )
	{
		registeredInterfaces_.erase( interface.second->getImpl() );
		delete interface.second;
	}
	if(parentContext_ == nullptr)
	{
		return;
	}
	for( auto & parentInterface : registeredInterfaces_ )
	{
		parentContext_->deregisterInterface( parentInterface );
	}
}

//==============================================================================
IInterface * DefaultContextManager::registerInterfaceImpl(
	const TypeId & id, IInterface * pImpl, ContextRegState regState )
{
	//Use pointer as unique id
	registeredInterfaces_.insert( pImpl );
	if (regState == Reg_Parent &&
		parentContext_ != nullptr)
	{
		return parentContext_->registerInterfaceImpl(
			id, pImpl, Reg_Local );
	}
	interfaces_.insert(
		std::make_pair(
			id, new RTTIHelper( pImpl ) ) );

	auto contextCreator =
		static_cast< IPluginContextCreator * >(
			pImpl->queryInterface( s_PluginContextCreatorTypeId ) );
	if(contextCreator)
	{
		for( auto & listener : listeners_ )
		{
			listener->onInterfaceRegistered( contextCreator );
		}
	}
	return pImpl;
}


//==============================================================================
bool DefaultContextManager::deregisterInterface(
	IInterface * pImpl )
{
	for( InterfaceMap::const_iterator it = interfaces_.begin();
		 it != interfaces_.end();
		 it++ )
	{
		if (pImpl != it->second->getImpl())
		{
			continue;
		}
		IPluginContextCreator * pluginContextCreator =
			static_cast< IPluginContextCreator * >(
				pImpl->queryInterface( s_PluginContextCreatorTypeId ) );
		if (pluginContextCreator)
		{
			for( auto & listener : listeners_ )
			{
				listener->onInterfaceDeregistered( pluginContextCreator );
			} 
		}
		delete it->second;
		interfaces_.erase( it );
		registeredInterfaces_.erase( pImpl );
		return true;
	}
	if (parentContext_ == nullptr)
	{
		return false;
	}
	bool deregistered =
		parentContext_->deregisterInterface( pImpl );
	if (deregistered)
	{
		registeredInterfaces_.erase( pImpl );
	}
	return deregistered;
}


//==============================================================================
void * DefaultContextManager::queryInterface( const TypeId & name )
{
	for( auto & interfaceIt : interfaces_ )
	{
		void * found = interfaceIt.second->queryInterface(
			name );
		if (found)
		{
			return found;
		}
	}
	if (parentContext_ == nullptr)
	{
		return nullptr;
	}
	return parentContext_->queryInterface( name );
}


//==============================================================================
void DefaultContextManager::queryInterface(
	const TypeId & name, std::vector< void * > & o_Impls )
{
	for( auto & interfaceIt : interfaces_ )
	{
		void * found =
			interfaceIt.second->queryInterface( name );
		if (found)
		{
			o_Impls.push_back( found );
		}
	}
	if (parentContext_ == nullptr)
	{
		return;
	}
	return parentContext_->queryInterface( name, o_Impls );
}


//==============================================================================
void DefaultContextManager::registerListener( IContextManagerListener & listener )
{
	listeners_.push_back( &listener );
}


//==============================================================================
void DefaultContextManager::deregisterListener(
	IContextManagerListener & listener )
{
	auto & listenerIt = std::find(
		listeners_.begin(), listeners_.end(), &listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}
