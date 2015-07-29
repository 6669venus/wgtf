#include "blueprint_editor_manager.hpp"
#include "blueprint_datasource.hpp"
#include "metadata/blueprint_model.mpp"

#include "qt_common/i_qt_framework.hpp"
#include "qt_common/shared_controls.hpp"

#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_application.hpp"
#include "dependency_system/i_interface.hpp"
#include "variant/variant.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_window.hpp"

#include <vector>

class BlueprintEditorPlugin : public PluginMain
{
private:
	BlueprintEditorManager blueprintEditorManager_;
	std::unique_ptr<BlueprintDataSource> dataSrc_;
	std::vector<IInterface*> types_;

public:
	BlueprintEditorPlugin(IComponentContext & contextManager)
	{
	}

	bool PostLoad(IComponentContext & contextManager)
	{
		IDefinitionManager* defManager = contextManager.queryInterface<IDefinitionManager>();
		if (defManager == NULL)
		{
			return false;
		}

		// register reflected type definition
		this->initReflectedTypes(*defManager);

		// register blueprint data source
		dataSrc_.reset(new BlueprintDataSource());
		types_.push_back(contextManager.registerInterface(dataSrc_.get(), false));

		// TODO: Fix. QueryInterface should NOT be called in PostLoad.
		// This is here temporarily because EditorShared::init
		// registers controls that invoke the variant system.
		Variant::setMetaTypeManager(contextManager.queryInterface<IMetaTypeManager>());

		// init BWControl
		SharedControls::init();

		return true;
	}

	void Initialise(IComponentContext & contextManager)
	{
		dataSrc_->init(contextManager);

		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();

		blueprintEditorManager_.init(*uiApplication, *uiFramework);
	}

	bool Finalise(IComponentContext & contextManager)
	{
		blueprintEditorManager_.fini();

		assert(dataSrc_);
		dataSrc_->fini(contextManager);

		return true;
	}

	void Unload(IComponentContext & contextManager)
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface(type);
		}
		dataSrc_ = nullptr;
	}

	void initReflectedTypes(IDefinitionManager & definitionManager)
	{
		SharedControls::initDefs(definitionManager);

		REGISTER_DEFINITION(BlueprintPage)
		REGISTER_DEFINITION(BlueprintModule)
		REGISTER_DEFINITION(BlueprintPropertyContainer)
		REGISTER_DEFINITION(BlueprintBaseProperty)
		REGISTER_DEFINITION(BlueprintFloatProperty)
		REGISTER_DEFINITION(BlueprintBooleanProperty)
		REGISTER_DEFINITION(BlueprintStringProperty)
	}
};

PLG_CALLBACK_FUNC(BlueprintEditorPlugin)

