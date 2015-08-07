#include "core_generic_plugin/generic_plugin.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection_utils/command_system_property_setter.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/set_reflectedpropertyrootobject_command.hpp"

#include "core_reflection_utils/reflected_types.hpp"

//==============================================================================
class EditorInteractionPlugin
	: public PluginMain
{
private:
	std::unique_ptr< SetReflectedPropertyCommand > setReflectedPropertyCmd_;
	std::unique_ptr< SetReflectedPropertyRootObjectCommand > setReflectedPropertyRootObjCmd_;
	std::unique_ptr< CommandSystemReflectionPropertySetter > commandSystemReflectionPropertySetter_;
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	EditorInteractionPlugin( IComponentContext & contextManager )
		: setReflectedPropertyCmd_( new SetReflectedPropertyCommand )
		, setReflectedPropertyRootObjCmd_(new SetReflectedPropertyRootObjectCommand())
	{
		
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager ) override
	{
		commandSystemReflectionPropertySetter_.reset( 
			new CommandSystemReflectionPropertySetter() );
		types_.push_back( contextManager.registerInterface( 
			commandSystemReflectionPropertySetter_.get(), false ) );

		return true;
	}


	//==========================================================================
	void Initialise( IComponentContext & contextManager ) override
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

		auto commandSystemProvider = contextManager.queryInterface< ICommandManager >();
		if (commandSystemProvider)
		{
			commandSystemReflectionPropertySetter_->init( *commandSystemProvider );
			commandSystemProvider->registerCommand( setReflectedPropertyCmd_.get() );
			commandSystemProvider->registerCommand(setReflectedPropertyRootObjCmd_.get());
		}
	}


	//==========================================================================
	bool Finalise(IComponentContext & contextManager) override
	{
		auto commandSystemProvider = contextManager.queryInterface< ICommandManager >();
		if (commandSystemProvider)
		{
			commandSystemProvider->deregisterCommand( setReflectedPropertyCmd_->getId() );
			commandSystemProvider->deregisterCommand(setReflectedPropertyRootObjCmd_->getId());
			setReflectedPropertyCmd_ = nullptr;
			setReflectedPropertyRootObjCmd_ = nullptr;
		}

		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
		commandSystemReflectionPropertySetter_.reset();
	}
};

PLG_CALLBACK_FUNC( EditorInteractionPlugin )