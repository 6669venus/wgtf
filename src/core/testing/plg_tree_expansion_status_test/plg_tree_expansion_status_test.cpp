#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_variant/variant.hpp"

//#include "core_ui_framework/i_ui_application.hpp"
//#include "core_ui_framework/i_ui_framework.hpp"
//#include "core_ui_framework/i_window.hpp"
//#include "tree_list_model.mpp"

#include <vector>


//==============================================================================
class TreeExpansionTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	TreeExpansionTestPlugin(IComponentContext & contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		//IDefinitionManager* defManager =
		//	contextManager.queryInterface< IDefinitionManager >();
		//if (defManager == NULL)
		//{
		//	return false;
		//}
		//this->initReflectedTypes( *defManager );
		//// TODO: Fix. QueryInterface should NOT be called in PostLoad.
		//// This is here temporarily because EditorShared::init
		//// registers controls that invoke the variant system.
		//Variant::setMetaTypeManager( 
		//	contextManager.queryInterface< IMetaTypeManager >() );

		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		//auto uiApplication = contextManager.queryInterface< IUIApplication >();
		//auto uiFramework = contextManager.queryInterface< IUIFramework >();

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		//for (auto type: types_)
		//{
		//	contextManager.deregisterInterface( type );
		//}
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		//SharedControls::initDefs( definitionManager );
		//REGISTER_DEFINITION( TreeListModel )
	}

};


PLG_CALLBACK_FUNC( TreeExpansionTestPlugin )

