#include "generic_plugin/generic_plugin.hpp"

#include "command_system/command_system_provider.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection_utils/command_system_property_setter.hpp"
#include "reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "reflection_utils/reflected_types.hpp"

//==============================================================================
class EditorInteractionPlugin
	: public PluginMain
{
private:
	std::unique_ptr< SetReflectedPropertyCommand > setReflectedPropertyCmd_;
	std::unique_ptr< CommandSystemReflectionPropertySetter > commandSystemReflectionPropertySetter_;
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	EditorInteractionPlugin( IContextManager & contextManager )
		: setReflectedPropertyCmd_( new SetReflectedPropertyCommand )
	{
		
	}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager ) override
	{
		commandSystemReflectionPropertySetter_.reset( 
			new CommandSystemReflectionPropertySetter() );
		types_.push_back( contextManager.registerInterface( 
			commandSystemReflectionPropertySetter_.get(), false ) );

		return true;
	}


	//==========================================================================
	void Initialise( IContextManager & contextManager ) override
	{
		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		assert( metaTypeMgr != nullptr );
		Variant::setMetaTypeManager( metaTypeMgr );

		auto defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == nullptr)
		{
			return;
		}
		IDefinitionManager & definitionManager = *defManager;
		Reflection_Utils::initReflectedTypes( definitionManager );

		auto commandSystemProvider = contextManager.queryInterface< CommandSystemProvider >();
		if (commandSystemProvider)
		{
			commandSystemReflectionPropertySetter_->init( *commandSystemProvider );
			commandSystemProvider->registerCommand( setReflectedPropertyCmd_.get() );
		}
	}


	//==========================================================================
	bool Finalise(IContextManager & contextManager) override
	{
		auto commandSystemProvider = contextManager.queryInterface< CommandSystemProvider >();
		if (commandSystemProvider)
		{
			commandSystemProvider->deregisterCommand( setReflectedPropertyCmd_->getId() );
			setReflectedPropertyCmd_ = nullptr;
		}

		return true;
	}

	//==========================================================================
	void Unload( IContextManager & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
		commandSystemReflectionPropertySetter_.reset();
	}
};

PLG_CALLBACK_FUNC( EditorInteractionPlugin )