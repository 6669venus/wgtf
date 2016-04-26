#include "core_generic_plugin/generic_plugin.hpp"

#include "history_object.hpp"
#include "metadata/history_object.mpp"
#include "metadata/display_object.mpp"
#include "core_command_system/i_command_manager.hpp"

#include "core_logging/logging.hpp"

#include "core_qt_common/i_qt_framework.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_action.hpp"


class HistoryUIPlugin
	: public PluginMain
{
public:
	HistoryUIPlugin( IComponentContext& contextManager )
	{
	}

	void createActions( IUIFramework & uiFramework, IUIApplication & uiApplication )
	{
		// hook undo/redo
		using namespace std::placeholders;

		undo_ = uiFramework.createAction(
			"Undo", 
			std::bind( &HistoryUIPlugin::undo, this, _1 ),
			std::bind( &HistoryUIPlugin::canUndo, this, _1 ) );

		redo_ = uiFramework.createAction(
			"Redo", 
			std::bind( &HistoryUIPlugin::redo, this, _1 ),
			std::bind( &HistoryUIPlugin::canRedo, this, _1 ) );

		uiApplication.addAction( *undo_ );
		uiApplication.addAction( *redo_ );
	}

	void destroyActions( IUIApplication & uiApplication )
	{
		uiApplication.removeAction( *redo_ );
		uiApplication.removeAction( *undo_ );
		redo_.reset();
		undo_.reset();
	}

	void undo( IAction * action )
	{
		assert( commandSystemProvider_ );
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}
		commandSystemProvider_->undo();
	}

	void redo( IAction * action )
	{
		assert( commandSystemProvider_ );
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}
		commandSystemProvider_->redo();
	}

	bool canUndo( const IAction * action) const
	{
		if (commandSystemProvider_ == nullptr)
		{
			return false;
		}
		return commandSystemProvider_->canUndo();
	}

	bool canRedo( const IAction * action ) const
	{
		if (commandSystemProvider_ == nullptr)
		{
			return false;
		}
		return commandSystemProvider_->canRedo();
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

		commandSystemProvider_ =
			Context::queryInterface< ICommandManager >();
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}

		auto pHistoryDefinition = pDefinitionManager->getDefinition(
			getClassIdentifier< HistoryObject >() );
		assert( pHistoryDefinition != nullptr );
		history_ = pHistoryDefinition->create();
		history_.getBase< HistoryObject >()->init( *commandSystemProvider_, definitionManager );

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

		if (panel_ != nullptr)
		{
			uiApplication->addView( *panel_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}

		createActions( *pQtFramework, *uiApplication );
	}

	bool Finalise( IComponentContext& contextManager ) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return true;
		}
		if (panel_ != nullptr)
		{
			uiApplication->removeView( *panel_ );
			panel_ = nullptr;
		}
		destroyActions( *uiApplication );
		auto historyObject = history_.getBase< HistoryObject >();
		assert( historyObject != nullptr );
		historyObject->fini();
		return true;
	}

private:
	std::unique_ptr< IView > panel_;
	ObjectHandle history_;

	ICommandManager * commandSystemProvider_;
	std::unique_ptr< IAction > undo_;
	std::unique_ptr< IAction > redo_;
};


PLG_CALLBACK_FUNC( HistoryUIPlugin )

