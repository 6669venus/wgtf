#include "generic_plugin/generic_plugin.hpp"

#include "history_object.hpp"
#include "metadata/history_object.mpp"

#include "command_system/command_system_provider.hpp"

#include "qt_common/i_qt_framework.hpp"

#include "reflection/reflection_macros.hpp"
#include "reflection/type_class_definition.hpp"

#include "ui_framework/i_view.hpp"
#include "ui_framework/i_ui_application.hpp"


class HistoryUIPlugin
	: public PluginMain
{
public:
	HistoryUIPlugin( IContextManager& contextManager )
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

		auto pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return;
		}
		auto& definitionManager = *pDefinitionManager;
		REGISTER_DEFINITION( HistoryObject )

		CommandSystemProvider* pCommandSystemProvider =
			Context::queryInterface< CommandSystemProvider >();
		if (pCommandSystemProvider == nullptr)
		{
			return;
		}
		auto & history = const_cast< GenericList & >(
			pCommandSystemProvider->getHistory() );

		auto pHistoryDefinition = pDefinitionManager->getDefinition(
			getClassIdentifier< HistoryObject >() );
		assert( pHistoryDefinition != nullptr );
		history_ = pHistoryDefinition->create();
		history_.getBase< HistoryObject >()->init( *pCommandSystemProvider );

		auto pQtFramework = contextManager.queryInterface< IQtFramework >();
		if (pQtFramework == nullptr)
		{
			return;
		}

		panel_ = pQtFramework->createView( 
			"qrc:///plg_history_ui/WGHistoryView.qml",
			IUIFramework::ResourceType::Url, history_ );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		uiApplication->addView( *panel_ );
	}

	bool Finalise( IContextManager& contextManager ) override
	{
		panel_ = nullptr;

		return true;
	}

private:
	std::unique_ptr< IView > panel_;
	ObjectHandle history_;
};


PLG_CALLBACK_FUNC( HistoryUIPlugin )

