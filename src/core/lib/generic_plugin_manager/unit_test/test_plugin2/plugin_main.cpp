#include "generic_plugin/generic_plugin.hpp"

#include "reflection/i_definition_manager.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/metadata/meta_types.hpp"

#include "plugin_objects.hpp"

class TestPlugin2
	: public PluginMain
{
public:
	TestPlugin2( IContextManager & contextManager )
	{
	}

	bool PostLoad( IContextManager & contextManager )
	{
		IDefinitionManager* pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return false;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( TestPlugin2TestObject );
		REGISTER_DEFINITION( TestPlugin2Interface );

		pInterface_ = definitionManager.create<TestPlugin2Interface>();
		types_.push_back( 
			contextManager.registerInterface( pInterface_.get(), false ) );

		return (pInterface_ != nullptr);
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
	ObjectHandleT< TestPlugin2Interface > pInterface_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( TestPlugin2 )
