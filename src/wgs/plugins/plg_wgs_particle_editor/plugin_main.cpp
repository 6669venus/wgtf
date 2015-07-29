#include "dependency_system/i_interface.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "main_window/main_window.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "qt_common/qt_window.hpp"
#include "qt_common/shared_controls.hpp"

#include "ui_framework/i_window.hpp"
#include "ui_framework/i_view.hpp"
#include "interfaces/i_engine_adapter.hpp"

#include "variant/interfaces/i_meta_type_manager.hpp"
#include "variant/variant.hpp"

//==============================================================================
class ParticleEditorPlugin
	: public PluginMain
{
private:
	MainWindow mainWindow_;

public:
	//==========================================================================
	ParticleEditorPlugin(IComponentContext & contextManager)
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		IDefinitionManager* defManager = contextManager.queryInterface< IDefinitionManager >();
		SharedControls::initDefs(*defManager);
		SharedControls::init();

		

		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		IEngineAdapter* engine = contextManager.queryInterface<IEngineAdapter>();

		if (engine == nullptr)
			return;

		if (!engine->initApp("Moonshine"))
		{
			// failed to initialize gamecentralutils.dll?
			return; 
		}

		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		assert(metaTypeMgr != nullptr);
		Variant::setMetaTypeManager(metaTypeMgr);
		mainWindow_.init( contextManager );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		mainWindow_.fini();
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}
};


PLG_CALLBACK_FUNC(ParticleEditorPlugin)
