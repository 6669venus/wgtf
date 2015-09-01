#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "qt_framework_adapter.hpp"
#include "qt_application_adapter.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include <vector>
#include <QApplication>

class MayaAdapterPlugin
	: public PluginMain
{
public:
	MayaAdapterPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();

		if (pPluginContextManager && pPluginContextManager->getExecutablePath())
			QCoreApplication::addLibraryPath(pPluginContextManager->getExecutablePath());
		
		qtApplication_ = new QtApplicationAdapter();
		qtFramework_ = new QtFrameworkAdapter();

		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		qtFramework_->initialise( contextManager );

		SharedControls::init();

		qtApplication_->initialise( qtFramework_ );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		qtApplication_->finalise();
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
		qtApplication_ = nullptr;
	}

private:
	QtFramework * qtFramework_;
    QtApplication * qtApplication_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( MayaAdapterPlugin )

