#include "core_generic_plugin/generic_plugin.hpp"

#include "core_data_model/i_list_model.hpp"
#include "macros_object.hpp"
#include "metadata/macros_object.mpp"

#include "core_command_system/i_command_manager.hpp"

#include "core_logging/logging.hpp"

#include "core_qt_common/i_qt_framework.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"


namespace wgt
{
class MacrosUIPlugin
	: public PluginMain
{
public:
	MacrosUIPlugin( IComponentContext& contextManager )
	{
	}

	bool PostLoad( IComponentContext& contextManager ) override
	{
		return true;
	}

	void Initialise( IComponentContext& contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		auto uiApplication = Context::queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}
		
		auto qtFramework = Context::queryInterface< IQtFramework >();
		if (qtFramework == nullptr)
		{
			return;
		}

		auto pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return;
		}
		auto& definitionManager = *pDefinitionManager;
		REGISTER_DEFINITION( MacrosObject )

		ICommandManager* pCommandSystemProvider =
			Context::queryInterface< ICommandManager >();
		if (pCommandSystemProvider == nullptr)
		{
			return;
		}

		auto pMacroDefinition = pDefinitionManager->getDefinition(
			getClassIdentifier< MacrosObject >() );
		assert( pMacroDefinition != nullptr );
		macros_ = pMacroDefinition->create();
		macros_.getBase< MacrosObject >()->init( *pCommandSystemProvider );

		panel_ = qtFramework->createView( 
			"WGMacros/WGMacroView.qml",
			IUIFramework::ResourceType::Url, macros_ );

		if (panel_ != nullptr)
		{
			uiApplication->addView( *panel_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}
	}

	bool Finalise( IComponentContext& contextManager ) override
	{
		auto uiApplication = Context::queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return true;
		}
		if (panel_ != nullptr)
		{
			uiApplication->removeView( *panel_ );
			panel_ = nullptr;
		}

		return true;
	}

private:
	std::unique_ptr< IView > panel_;
	ObjectHandle macros_;
};

PLG_CALLBACK_FUNC( MacrosUIPlugin )

} // end namespace wgt
