#include "generic_plugin/generic_plugin.hpp"

#include "macros_object.hpp"
#include "metadata/macros_object.mpp"

#include "command_system/command_system_provider.hpp"

#include "qt_common/i_qt_framework.hpp"

#include "reflection/reflection_macros.hpp"
#include "reflection/type_class_definition.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_view.hpp"


class MacrosUIPlugin
	: public PluginMain
{
public:
	MacrosUIPlugin( IContextManager& contextManager )
	{
	}

	bool PostLoad( IContextManager& contextManager ) override
	{
		return true;
	}

	void Initialise( IContextManager& contextManager ) override
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
			"qrc:///plg_macros_ui/WGMacroView.qml",
			IUIFramework::ResourceType::Url, macros_ );

		uiApplication->addView( *panel_ );
	}

	bool Finalise( IContextManager& contextManager ) override
	{
		panel_ = nullptr;

		return true;
	}

private:
	std::unique_ptr< IView > panel_;
	ObjectHandle macros_;
};


PLG_CALLBACK_FUNC( MacrosUIPlugin )

