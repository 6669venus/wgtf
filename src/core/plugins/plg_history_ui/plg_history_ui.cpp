#include "core_generic_plugin/generic_plugin.hpp"

#include "history_object.hpp"
#include "metadata/history_object.mpp"
#include "metadata/display_object.mpp"
#include "core_command_system/i_command_manager.hpp"

#include "core_qt_common/i_qt_framework.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"


class HistoryUIPlugin
	: public PluginMain
{
public:
	HistoryUIPlugin( IComponentContext& contextManager )
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

		auto pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return;
		}
		auto& definitionManager = *pDefinitionManager;
		REGISTER_DEFINITION( HistoryObject );
		REGISTER_DEFINITION( DisplayObject );

		ICommandManager* pCommandSystemProvider =
			Context::queryInterface< ICommandManager >();
		if (pCommandSystemProvider == nullptr)
		{
			return;
		}
		auto & history = const_cast< VariantList & >(
			pCommandSystemProvider->getHistory() );

		auto pHistoryDefinition = pDefinitionManager->getDefinition(
			getClassIdentifier< HistoryObject >() );
		assert( pHistoryDefinition != nullptr );
		history_ = pHistoryDefinition->create();
		history_.getBase< HistoryObject >()->init( *pCommandSystemProvider, definitionManager );

		auto pQtFramework = contextManager.queryInterface< IQtFramework >();
		if (pQtFramework == nullptr)
		{
			return;
		}

		panel_ = pQtFramework->createView( 
			"WGHistory/WGHistoryView.qml",
			IUIFramework::ResourceType::Url, history_ );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		uiApplication->addView( *panel_ );
	}

	bool Finalise( IComponentContext& contextManager ) override
	{
		auto historyObject = history_.getBase< HistoryObject >();
		assert( historyObject != nullptr );
		historyObject->fini();
		panel_ = nullptr;

		return true;
	}

private:
	std::unique_ptr< IView > panel_;
	ObjectHandle history_;
};


PLG_CALLBACK_FUNC( HistoryUIPlugin )

