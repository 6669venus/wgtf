#include "generic_plugin/generic_plugin.hpp"
#include "variant/variant.hpp"
#include "copy_paste_system/copy_paste_manager.hpp"
//#include "copy_paste_system/i_copyable_object.hpp"
#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"

//==============================================================================
class CopyPastePlugin
	: public PluginMain
{
private:
	std::unique_ptr< CopyPasteManager > copyPasteManager_;
	std::vector<IInterface*> types_;
	std::unique_ptr< IAction > copy_;
	std::unique_ptr< IAction > paste_;

	void createCopyPasteUI( IContextManager & contextManager )
	{
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );
		uiFramework->loadActionData( 
			":/actiondata",
			IUIFramework::ResourceType::File );
		copy_ = uiFramework->createAction(
			"Copy", 
			std::bind( &CopyPastePlugin::copy, this ),
			std::bind( &CopyPastePlugin::canCopy, this ) );

		paste_ = uiFramework->createAction( 
			"Paste", 
			std::bind( &CopyPastePlugin::paste, this ),
			std::bind( &CopyPastePlugin::canPaste, this ) );

		uiApplication->addAction( *copy_ );
		uiApplication->addAction( *paste_ );

	}

	void destroyActions()
	{
		paste_.reset();
		copy_.reset();
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
		return copyPasteManager_->canCopy();
	}

	bool canPaste() const
	{
		return copyPasteManager_->canPaste();
	}

public:
	//==========================================================================
	CopyPastePlugin( IContextManager & contextManager )
		: copyPasteManager_( new CopyPasteManager )
	{
	}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager ) override
	{
		types_.push_back( contextManager.registerInterface( copyPasteManager_.get(), false ) );
		return true;
	}


	//==========================================================================
	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );
		auto serializationManager = contextManager.queryInterface<ISerializationManager>();
		copyPasteManager_->init( serializationManager );

		createCopyPasteUI( contextManager );
	}


	//==========================================================================
	bool Finalise(IContextManager & contextManager) override
	{
		destroyActions();
		copyPasteManager_->fini();
		return true;
	}

	//==========================================================================
	void Unload( IContextManager & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
		copyPasteManager_ = nullptr;
	}
};

PLG_CALLBACK_FUNC( CopyPastePlugin )