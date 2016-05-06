#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_action.hpp"

#include "core_data_model/i_list_model.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_data_model/generic_list.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

#include "tinyxml2.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"

#include "demo_objects.mpp"

#include <stdio.h>
#include "core_command_system/i_env_system.hpp"
#include "core_serialization/i_file_system.hpp"

namespace
{
	enum class ModelPropertyValueType : uint8_t
	{
		String = 0,
		Boolean,
		Integer,
		Double,
		EnumList,
		Vector2,
		Vector3,
		Vector4,
	};
}

class DemoDoc: public IViewEventListener
{
public:
	DemoDoc( const char* name, IEnvManager* envManager, IUIFramework* uiFramework,
		IUIApplication* uiApplication, ObjectHandle demo );
	~DemoDoc();

	// IViewEventListener
	virtual void onFocusIn( IView* view ) override;
	virtual void onFocusOut( IView* view ) override;

private:
	IEnvManager* envManager_;
	IUIApplication* uiApplication_;
	std::unique_ptr< IView > centralView_;
	int envId_;
};

DemoDoc::DemoDoc(const char* name, IEnvManager* envManager, IUIFramework* uiFramework,
								IUIApplication* uiApplication, ObjectHandle demo)
	: envManager_( envManager )
	, uiApplication_(uiApplication)
{
	envId_ = envManager_->addEnv( name );
    envManager_->loadEnvState( envId_ );
	envManager_->selectEnv( envId_ );

	centralView_ = uiFramework->createView( "plg_demo_test/demo.qml", IUIFramework::ResourceType::Url, demo );
	centralView_->registerListener( this );
	if (centralView_ != nullptr)
	{
		uiApplication->addView( *centralView_ );
	}
	else
	{
		NGT_ERROR_MSG( "Failed to load qml\n" );
	}
}

DemoDoc::~DemoDoc()
{
	if (centralView_ != nullptr)
	{
		uiApplication_->removeView( *centralView_ );
		centralView_->deregisterListener( this );
	}
    envManager_->saveEnvState( envId_ );
	envManager_->removeEnv( envId_ );
}

void DemoDoc::onFocusIn(IView* view)
{
    if(view != centralView_.get())
    {
        return;
    }
	envManager_->selectEnv( envId_ );
}

void DemoDoc::onFocusOut(IView* view)
{
}

//==============================================================================
class DemoTestPlugin
	: public PluginMain
{
private:
	
	std::unique_ptr< DemoDoc > demoDoc_;
	std::unique_ptr< DemoDoc > demoDoc2_;
	std::unique_ptr< IView > propertyView_;
	std::unique_ptr< IView > sceneBrowser_;
	std::unique_ptr< IView > viewport_;
	ObjectHandle demoModel_;

	IReflectionController* controller_;
	IDefinitionManager* defManager_;
	std::unique_ptr< IAction > createAction_;

public:
	//==========================================================================
	DemoTestPlugin(IComponentContext & contextManager )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		defManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager_ != nullptr);
		this->initReflectedTypes( *defManager_ );

		controller_ = contextManager.queryInterface< IReflectionController >();

		demoModel_ = defManager_->create<DemoObjects>();
		demoModel_.getBase< DemoObjects >()->init( contextManager );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		auto envManager = contextManager.queryInterface< IEnvManager >();
		if ( uiApplication == nullptr || uiFramework == nullptr || envManager == nullptr )
		{
			return;
		}

		demoDoc_.reset( new DemoDoc("sceneModel0", envManager, uiFramework, uiApplication, demoModel_) );
		demoDoc2_.reset( new DemoDoc("sceneModel1", envManager, uiFramework, uiApplication, demoModel_) );

		propertyView_ = uiFramework->createView( 
			"plg_demo_test/demo_property_panel.qml", 
			IUIFramework::ResourceType::Url, demoModel_ );

		if (propertyView_ != nullptr)
		{
			uiApplication->addView( *propertyView_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}

		sceneBrowser_ = uiFramework->createView( 
			"plg_demo_test/demo_list_panel.qml", 
			IUIFramework::ResourceType::Url, demoModel_ );

		if (sceneBrowser_ != nullptr)
		{
			uiApplication->addView( *sceneBrowser_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}

		viewport_ = uiFramework->createView(
			"plg_demo_test/Framebuffer.qml",
			IUIFramework::ResourceType::Url, demoModel_ );

		if (viewport_ != nullptr)
		{
			uiApplication->addView( *viewport_ );
		}
		else
		{
			NGT_ERROR_MSG( "Failed to load qml\n" );
		}

		createAction_ = uiFramework->createAction(
			"New Object", 
			[&] (const IAction * action) { createObject(); },
			[&] (const IAction * action) { return canCreate(); } );

		uiApplication->addAction( *createAction_ );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		if (propertyView_ != nullptr)
		{
			uiApplication->removeView( *propertyView_ );
		}
		if (sceneBrowser_ != nullptr)
		{
			uiApplication->removeView( *sceneBrowser_ );
		}
		if (viewport_ != nullptr)
		{
			uiApplication->removeView( *viewport_ );
		}

		propertyView_ = nullptr;
		sceneBrowser_ = nullptr;
		viewport_ = nullptr;
		demoDoc_ = nullptr;
		demoDoc2_ = nullptr;
		demoModel_.getBase< DemoObjects >()->fini();
		demoModel_ = nullptr;
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		REGISTER_DEFINITION( DemoObjects )
	}

private:
	void createObject()
	{
		IClassDefinition* def = defManager_->getDefinition<DemoObjects>();
		PropertyAccessor pa = def->bindProperty( "New Object", demoModel_ );
		assert( pa.isValid() );
		ReflectedMethodParameters parameters;
		parameters.push_back( Vector3( 0.f, 0.f, -10.f) );
		Variant returnValue = controller_->invoke( pa, parameters );

		/*std::unique_ptr<ReflectedMethodCommandParameters> commandParameters( new ReflectedMethodCommandParameters() );
		commandParameters->setId( key.first );
		commandParameters->setPath( key.second.c_str() );
		commandParameters->setParameters( parameters );

		commandManager_->queueCommand(
			getClassIdentifier<InvokeReflectedMethodCommand>(), ObjectHandle( std::move( commandParameters ),
			pa.getDefinitionManager()->getDefinition<ReflectedMethodCommandParameters>() ) )

		demoModel_.getBase< DemoObjects >()->createObject();*/
	}

	bool canCreate() { return true; }

};


PLG_CALLBACK_FUNC(DemoTestPlugin)

