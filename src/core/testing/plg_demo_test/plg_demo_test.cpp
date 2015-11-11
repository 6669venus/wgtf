#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "tinyxml2.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "demo_objects.mpp"
#include <stdio.h>
#include "core_command_system/i_env_system.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

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
	DemoDoc( IEnvManager* envManager, IUIFramework* uiFramework, IUIApplication* uiApplication, ObjectHandle demo );
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

DemoDoc::DemoDoc(IEnvManager* envManager, IUIFramework* uiFramework, IUIApplication* uiApplication, ObjectHandle demo)
	: envManager_( envManager )
	, uiApplication_(uiApplication)
{
	envId_ = envManager_->addEnv();
	envManager_->selectEnv( envId_ );
	demo.getBase< DemoObjects >()->loadDemoData( envId_ );

	centralView_ = uiFramework->createView( "plg_demo_test/demo.qml", IUIFramework::ResourceType::Url, demo );
	centralView_->registerListener( this );
	uiApplication->addView( *centralView_ );
}

DemoDoc::~DemoDoc()
{
	uiApplication_->removeView( *centralView_ );
	centralView_->deregisterListener( this );

	envManager_->selectEnv( envId_ );
	envManager_->removeEnv( envId_ );
}

void DemoDoc::onFocusIn(IView* view)
{
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
	std::unique_ptr< IView > senceBrowser_;
	std::unique_ptr< IView > viewport_;
	ObjectHandle demoModel_;

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

		IDefinitionManager* defManager =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager != nullptr);
		this->initReflectedTypes( *defManager );

		auto demoModelDefinition = defManager->getDefinition(
			getClassIdentifier< DemoObjects >() );
		assert( demoModelDefinition != nullptr );
		demoModel_ = demoModelDefinition->create();
		demoModel_.getBase< DemoObjects >()->init( contextManager );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		auto envManager = contextManager.queryInterface< IEnvManager >();
		if ( uiApplication == nullptr || uiFramework == nullptr || envManager == nullptr )
		{
			return;
		}

		demoDoc_.reset( new DemoDoc(envManager, uiFramework, uiApplication, demoModel_) );
		demoDoc2_.reset( new DemoDoc(envManager, uiFramework, uiApplication, demoModel_) );

		propertyView_ = uiFramework->createView( 
			"plg_demo_test/demo_property_panel.qml", 
			IUIFramework::ResourceType::Url, demoModel_ );

		uiApplication->addView( *propertyView_ );

		senceBrowser_ = uiFramework->createView( 
			"plg_demo_test/demo_list_panel.qml", 
			IUIFramework::ResourceType::Url, demoModel_ );

		uiApplication->addView( *senceBrowser_ );

		viewport_ = uiFramework->createView(
			"plg_demo_test/Framebuffer.qml",
			IUIFramework::ResourceType::Url, demoModel_ );

		uiApplication->addView( *viewport_ );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		uiApplication->removeView( *propertyView_ );
		uiApplication->removeView( *senceBrowser_ );
		uiApplication->removeView( *viewport_ );

		demoDoc_ = nullptr;
		demoDoc2_ = nullptr;
		propertyView_ = nullptr;
		senceBrowser_ = nullptr;
		viewport_ = nullptr;
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

};


PLG_CALLBACK_FUNC(DemoTestPlugin)

