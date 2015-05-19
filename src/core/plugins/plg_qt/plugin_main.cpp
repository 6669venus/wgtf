#include "generic_plugin_system/generic_plugin.hpp"
#include "generic_plugin_system/generic_plugin_manager.hpp"

#include "qt_application.hpp"
#include "qt_framework.hpp"
#include "variant/variant.hpp"

#include <vector>

class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IContextManager & contextManager ){}

	bool PostLoad( IContextManager & contextManager ) override
	{
		qtApplication_ = new QtApplication();
		qtFramework_ = new QtFramework();
		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		qtFramework_->initialise( contextManager );
		qtApplication_->initialise( qtFramework_ );
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		qtApplication_->finalise();
		qtFramework_->finalise();

		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
		qtApplication_ = nullptr;
		qtFramework_ = nullptr;
	}

private:
	QtApplication * qtApplication_;
	QtFramework * qtFramework_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

