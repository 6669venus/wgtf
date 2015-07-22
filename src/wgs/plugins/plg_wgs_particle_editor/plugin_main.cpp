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
	ParticleEditorPlugin(IContextManager & contextManager)
	{
	}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		IDefinitionManager* defManager = contextManager.queryInterface< IDefinitionManager >();
		SharedControls::initDefs(*defManager);
		SharedControls::init();

		

		return true;
	}

	//==========================================================================
	void Initialise( IContextManager & contextManager )
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
	bool Finalise( IContextManager & contextManager )
	{
		mainWindow_.fini();
		return true;
	}

	//==========================================================================
	void Unload( IContextManager & contextManager )
	{
	}
};


PLG_CALLBACK_FUNC(ParticleEditorPlugin)
