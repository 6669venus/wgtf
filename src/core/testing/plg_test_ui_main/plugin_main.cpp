#include "dependency_system/i_interface.hpp"
#include "qt_common/shared_controls.hpp"
#include "generic_plugin/interfaces/i_application.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "test_ui/test_ui.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "test_datasource.hpp"
#include "testing/reflection_test_objects/test_objects.hpp"
#include "variant/variant.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_window.hpp"


#include "pages/metadata/test_page.mpp"
#include "pages/metadata/test_polymorphism.mpp"
#include "variant/variant.hpp"
#include "testing/reflection_test_objects/test_objects.hpp"

#include <vector>


//==============================================================================
class MainUITestPlugin
	: public PluginMain
{
private:
	TestUI testUI_;
	std::unique_ptr< TestDataSource > dataSrc_;
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	MainUITestPlugin(IComponentContext & contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		IDefinitionManager* defManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (defManager == NULL)
		{
			return false;
		}

		// register reflected type definition
		TestObjects testObjects;
		this->initReflectedTypes( *defManager );
		testObjects.initDefs( *defManager );

		// register test data source
		dataSrc_.reset( new TestDataSource() );
		types_.push_back( contextManager.registerInterface( dataSrc_.get(), false ) );

		// TODO: Fix. QueryInterface should NOT be called in PostLoad.
		// This is here temporarily because EditorShared::init
		// registers controls that invoke the variant system.
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );
		// init BWControl
		SharedControls::init();

		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		dataSrc_->init( contextManager );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();

		testUI_.init( *uiApplication, *uiFramework );
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		testUI_.fini();

		assert( dataSrc_ );
		dataSrc_->fini( contextManager );

		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
		dataSrc_ = nullptr;
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		SharedControls::initDefs( definitionManager );
		REGISTER_DEFINITION( TestPolyCheckBox )
		REGISTER_DEFINITION( TestPolyTextField )
		REGISTER_DEFINITION( TestPolyComboBox )
		REGISTER_DEFINITION( TestPolyColor3 )
		REGISTER_DEFINITION( TestPage )
		REGISTER_DEFINITION( TestPage2 )
	}

};


PLG_CALLBACK_FUNC( MainUITestPlugin )

