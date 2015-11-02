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

//==============================================================================
class DemoTestPlugin
	: public PluginMain
{
private:
	std::unique_ptr< IView > centralView_;
	std::unique_ptr< IView > propertyView_;
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
		if ( uiApplication == nullptr || 
			uiFramework == nullptr )
		{
			return ;
		}
		centralView_ = uiFramework->createView( 
			"plg_demo_test/demo.qml",
			IUIFramework::ResourceType::Url,  demoModel_ );

		uiApplication->addView( *centralView_ );

		propertyView_ = uiFramework->createView( 
			"plg_demo_test/demo_property_panel.qml", 
			IUIFramework::ResourceType::Url, demoModel_ );

		uiApplication->addView( *propertyView_ );

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
		uiApplication->removeView( *centralView_ );
		propertyView_ = nullptr;
		centralView_ = nullptr;
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

