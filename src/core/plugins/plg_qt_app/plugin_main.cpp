#include "generic_plugin/generic_plugin.hpp"

#include "qt_application.hpp"
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
		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		IQtFramework* qtFramework =
			contextManager.queryInterface< IQtFramework >();
		assert( qtFramework != nullptr );

		qtApplication_->initialise( qtFramework );
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		qtApplication_->finalise();

		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtApplication_ = nullptr;
	}

private:
	QtApplication * qtApplication_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )

