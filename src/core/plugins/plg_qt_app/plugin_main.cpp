#include "core_generic_plugin/generic_plugin.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "qt_application.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_variant/variant.hpp"

#include <vector>
#include <QApplication>

class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();

		// This needs to be set before QtApplication gets constructed or else Qt won't know where to find platforms.
		// This will only occur when running from a plugin scenario such as Maya.
		if (pPluginContextManager && pPluginContextManager->getExecutablePath())
			QCoreApplication::addLibraryPath(pPluginContextManager->getExecutablePath());
		
		qtApplication_ = new QtApplication();
		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		IQtFramework* qtFramework =
			contextManager.queryInterface< IQtFramework >();
		assert( qtFramework != nullptr );

		qtApplication_->initialise( qtFramework );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		qtApplication_->finalise();

		return true;
	}

	void Unload( IComponentContext & contextManager ) override
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

