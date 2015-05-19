#include "generic_plugin_system/interfaces/i_context_manager.hpp"
#include "generic_plugin_system/interfaces/i_plugin_context_creator.hpp"
#include "generic_plugin_system/generic_plugin.hpp"
#include "generic_plugin_system/generic_plugin_manager.hpp"

#include "reflection/i_definition_manager.hpp"
#include "reflection/i_object_manager.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/reflection_macros.hpp"

#include "plugin_objects.hpp"


class TestPlugin1
	: public PluginMain
{
public:
	TestPlugin1( IContextManager & contextManager )
		: PluginMain()
	{
	}

	bool PostLoad( IContextManager & contextManager )
	{
		IObjectManager * pObjectManager = contextManager.queryInterface< IObjectManager >();
		if (pObjectManager == nullptr)
		{
			return false;
		}
		IDefinitionManager * pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return false;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( TestPlugin1TestObject );
		REGISTER_DEFINITION( TestPlugin1Interface );

		// Create plugin interface
		std::vector< const RefObjectId > objects;
		pObjectManager->getContextObjects( &definitionManager, 
			objects );

		// Search for the plugin interface
		for (std::vector< const RefObjectId >::iterator it = objects.begin(), 
			end = objects.end(); it != end; ++it)
		{
			auto & pObj = pObjectManager->getObject( *it );
			assert( pObj != nullptr );
			if (pObj.getDefinition() == 
				definitionManager.getDefinition< TestPlugin1Interface >())
			{
				pInterface_ = pObj;
				return true;
			}
		}

		// create new interface object
		pInterface_ = definitionManager.create< TestPlugin1Interface >();
		assert( pInterface_ != nullptr );

		// Do not delete when interface is unregistered
		// Reflection system needs to delete it
		types_.push_back( 
			contextManager.registerInterface( pInterface_.get(), false /*transferOwnership*/ ) );

		return true;
	}

	void Initialise( IContextManager & contextManager )
	{
		assert( pInterface_ != nullptr );
		ITestPlugin2 * plugin2 = 
			contextManager.queryInterface< ITestPlugin2 >();
		if (plugin2 != nullptr)
		{
			pInterface_->setObjectFromPlugin2( plugin2->getObject() );
		}
	}


	bool Finalise( IContextManager & contextManager )
	{
		if( pInterface_ != nullptr )
		{
			pInterface_->setObjectFromPlugin2( nullptr );
		}
		return true;
	}

	void Unload( IContextManager & contextManager )
	{
		for( auto type : types_ )
		{
			contextManager.deregisterInterface( type );
		}
		pInterface_ = nullptr;
	}

private:
	TestPlugin1InterfacePtr pInterface_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( TestPlugin1 )

