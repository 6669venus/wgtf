#include "core_generic_plugin/generic_plugin.hpp"

#include "copy_paste_manager.hpp"
#include "qt_framework.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"

#include <vector>

class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
        copyPasteManager_ = new CopyPasteManager();
		types_.push_back(
			contextManager.registerInterface( copyPasteManager_ ) );

		qtFramework_ = new QtFramework();
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		auto serializationManager = contextManager.queryInterface<ISerializationManager>();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();
		copyPasteManager_->init( serializationManager, commandsystem );

		qtFramework_->initialise( contextManager );

		SharedControls::init();
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
        copyPasteManager_->fini();
		qtFramework_->finalise();

		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtFramework_ = nullptr;
        copyPasteManager_ = nullptr;
	}

private:
	QtFramework * qtFramework_;
    CopyPasteManager * copyPasteManager_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

