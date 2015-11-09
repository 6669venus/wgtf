#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "tree_list_model.mpp"

#include <vector>


//==============================================================================
class TreeExpansionTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
	std::unique_ptr< IView > treeListView_;
	ObjectHandle treeListModel_;
public:
	//==========================================================================
	TreeExpansionTestPlugin(IComponentContext & contextManager )
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

		auto controller = contextManager.queryInterface<IReflectionController>();
		assert( controller != nullptr );
		
		auto treeListModelDefinition = defManager->getDefinition(
			getClassIdentifier< TreeListModel >() );
		assert( treeListModelDefinition != nullptr );
		treeListModel_ = treeListModelDefinition->create();
		treeListModel_.getBase< TreeListModel >()->init( *defManager, *controller );
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		auto pQtFramework = contextManager.queryInterface< IQtFramework >();
		if (pQtFramework != nullptr)
		{
			treeListView_ = pQtFramework->createView( "plg_tree_expansion_status_test/test_tree_list_panel.qml",
				IUIFramework::ResourceType::Url, treeListModel_ );
		}

		uiApplication->addView( *treeListView_ );

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeView( *treeListView_ );
		treeListView_ = nullptr;
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		REGISTER_DEFINITION( TreeListModel )
	}

};


PLG_CALLBACK_FUNC( TreeExpansionTestPlugin )

