#include "generic_plugin/generic_plugin.hpp"

#include "qt_framework.hpp"
#include "variant/variant.hpp"
#include "qt_common/shared_controls.hpp"
#include "qt_common/qt_new_handler.hpp"

#include <vector>

class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IContextManager & contextManager ){}

	bool PostLoad( IContextManager & contextManager ) override
	{
		qtFramework_ = new QtFramework();
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		qtFramework_->initialise( contextManager );

		SharedControls::init();
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		qtFramework_->finalise();

		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtFramework_ = nullptr;
	}

private:
	QtFramework * qtFramework_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

