
#include "src/type_registration.h"
#include "src/graph_editor.h"

#include <core_logging/logging.hpp>

#include <core_generic_plugin/interfaces/i_component_context.hpp>
#include <core_generic_plugin/generic_plugin.hpp>

#include <core_variant/variant.hpp>
#include <core_variant/interfaces/i_meta_type_manager.hpp>

#include <core_reflection/i_definition_manager.hpp>

#include <core_ui_framework/i_ui_framework.hpp>
#include <core_ui_framework/i_ui_application.hpp>
#include <core_ui_framework/i_view.hpp>

namespace wgt
{
class GraphEditorPlugin : public PluginMain
{
public:
	GraphEditorPlugin(IComponentContext& context)
	{
	}

	bool PostLoad(IComponentContext& context) override
	{
		return true;
	}

	void Initialise(IComponentContext& context) override
	{
		IUIFramework* uiFramework = context.queryInterface<IUIFramework>();
		IUIApplication* uiapplication = context.queryInterface<IUIApplication>();
		IDefinitionManager* defMng = context.queryInterface<IDefinitionManager>();

		assert(uiFramework != nullptr);
		assert(uiapplication != nullptr);
		assert(defMng != nullptr);

		Variant::setMetaTypeManager(context.queryInterface<IMetaTypeManager>());

		RegisterGrapEditorTypes(*defMng);
		editor = defMng->create<GraphEditor>(false);

		view = uiFramework->createView("plg_graph_editor/GraphEditorView.qml", IUIFramework::ResourceType::Url, std::move(editor));
		if (view != nullptr)
		{
			uiapplication->addView(*view);
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
	}

	bool Finalise(IComponentContext& context) override
	{
		const auto uiApplication = context.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		if (view != nullptr)
		{
			uiApplication->removeView( *view );
			view.reset();
		}
		editor = ObjectHandleT<GraphEditor>();
		return true;
	}

	void Unload(IComponentContext& context) override
	{
	}

private:
	std::unique_ptr<IView> view;
	ObjectHandleT<GraphEditor> editor;
};

PLG_CALLBACK_FUNC(GraphEditorPlugin)
}
