#include "generic_plugin/generic_plugin.hpp"

#include "qt_framework.hpp"
#include "variant/variant.hpp"

#include <vector>

class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		qtFramework_ = new QtFramework();
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		qtFramework_->initialise( contextManager );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
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
	}

private:
	QtFramework * qtFramework_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

