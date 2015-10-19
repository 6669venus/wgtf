#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_global_settings.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"

//==============================================================================
class CopyPastePlugin
	: public PluginMain
{
private:
	ICopyPasteManager * copyPasteManager_;
	IQtFramework * qtFramework;
	std::vector<IInterface*> types_;
	std::unique_ptr< IAction > toggleCopyControl_;
	std::unique_ptr< IAction > copy_;
	std::unique_ptr< IAction > paste_;

	void createCopyPasteUI( IComponentContext & contextManager )
	{
		qtFramework = contextManager.queryInterface<IQtFramework>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );
		uiFramework->loadActionData( 
			":/actiondata",
			IUIFramework::ResourceType::File );

		toggleCopyControl_ = uiFramework->createAction(
			"ToggleCopyControls", 
			std::bind( &CopyPastePlugin::toggleCopyControl, this ), 
			[] () { return true; },
			std::bind( &CopyPastePlugin::isCopyControlChecked, this ) );

        copyPasteManager_ = contextManager.queryInterface< ICopyPasteManager >();
        assert( copyPasteManager_ != nullptr );

		copy_ = uiFramework->createAction(
			"Copy", 
			std::bind( &CopyPastePlugin::copy, this ),
			std::bind( &CopyPastePlugin::canCopy, this ) );

		paste_ = uiFramework->createAction( 
			"Paste", 
			std::bind( &CopyPastePlugin::paste, this ),
			std::bind( &CopyPastePlugin::canPaste, this ) );

		uiApplication->addAction( *toggleCopyControl_ );
		uiApplication->addAction( *copy_ );
		uiApplication->addAction( *paste_ );

	}

	void destroyActions()
	{
		paste_.reset();
		copy_.reset();
		toggleCopyControl_.reset();
	}

	void toggleCopyControl()
	{
		assert( qtFramework != nullptr );
		auto globalSettings = qtFramework->qtGlobalSettings();
		bool enabled =  globalSettings->property( "wgCopyableEnabled" ).toBool();
		globalSettings->setProperty( "wgCopyableEnabled", !enabled );
	}

	bool isCopyControlChecked() const
	{
		assert( qtFramework != nullptr );
		auto globalSettings = qtFramework->qtGlobalSettings();
		return globalSettings->property( "wgCopyableEnabled" ).toBool();
	}

	void copy()
	{
		copyPasteManager_->copy();
	}

	void paste()
	{
		copyPasteManager_->paste();
	}

	bool canCopy() const
	{
		return isCopyControlChecked() && copyPasteManager_->canCopy();
	}

	bool canPaste() const
	{
		return isCopyControlChecked() && copyPasteManager_->canPaste();
	}

public:
	//==========================================================================
	CopyPastePlugin( IComponentContext & contextManager )
		: copyPasteManager_( nullptr )
		, qtFramework( nullptr )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager ) override
	{
		return true;
	}


	//==========================================================================
	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		createCopyPasteUI( contextManager );
	}


	//==========================================================================
	bool Finalise(IComponentContext & contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert( uiApplication != nullptr);
		uiApplication->removeAction( *toggleCopyControl_ );
		uiApplication->removeAction( *copy_ );
		uiApplication->removeAction( *paste_ );
		destroyActions();
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager ) override
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}
};

PLG_CALLBACK_FUNC( CopyPastePlugin )
