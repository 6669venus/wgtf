#include "balance_panel.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

#include "metadata/balance_panel_context.mpp"


BalancePanel::BalancePanel( IComponentContext & context )
	: Depends( context )
{
	this->createPythonObjects();
	this->createDataModel();
	this->addPanel();
}


BalancePanel::~BalancePanel()
{
	this->removePanel();
	this->destroyDataModel();
	this->destroyPythonObjects();
}


bool BalancePanel::createPythonObjects()
{
	auto pDefinitionManager = this->get< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	auto pScriptingEngine = this->get< IPythonScriptingEngine >();
	if (pScriptingEngine == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IPythonScriptingEngine\n" );
		return false;
	}
	auto & scriptingEngine = (*pScriptingEngine);

	if (!scriptingEngine.appendPath(
		L"..\\..\\..\\src\\wows\\plugins\\plg_maps_settings\\scripts" ))
	{
		NGT_ERROR_MSG( "Failed to append path\n" );
		return false;
	}

	ObjectHandle module = scriptingEngine.import( "MapsSettingsEditor" );
	if (!module.isValid())
	{
		NGT_ERROR_MSG( "Could not import module\n" );
		return false;
	}

	// MapsSettingsEditor.rootPythonObject
	auto moduleDefinition = module.getDefinition( definitionManager );
	auto pProperty = moduleDefinition->findProperty( "rootPythonObject" );
	if (pProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	auto rootObjectVariant = pProperty->get( module, definitionManager );
	const bool isRootObject = rootObjectVariant.tryCast< ObjectHandle >( rootPythonObject_ );
	if (!isRootObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	auto rootObjectDefinition = rootPythonObject_.getDefinition( definitionManager );
	auto pXMLDataProperty = rootObjectDefinition->findProperty( "mapsSettingsXMLData" );
	if (pXMLDataProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	auto xmlDataVariant = pXMLDataProperty->get( rootPythonObject_, definitionManager );
	const bool isXMLDataObject =
		xmlDataVariant.tryCast< Collection >( mapsSettingsXMLDataCollection_ );
	if (!isXMLDataObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	return true;
}


void BalancePanel::destroyPythonObjects()
{
	mapsSettingsXMLDataCollection_ = Collection();
	rootPythonObject_ = nullptr;
}


bool BalancePanel::createDataModel()
{
	auto pDefinitionManager = this->get< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	REGISTER_DEFINITION( BalancePanelContext )

	const bool managed = true;
	contextObject_ = definitionManager.create< BalancePanelContext >( managed );

	// Construct an IListModel from the scripts
	auto pMapsSettingsXMLDataModel = new CollectionModel();
	pMapsSettingsXMLDataModel->setSource( mapsSettingsXMLDataCollection_ );
	contextObject_->pMapsSettingsXMLDataModel_ = pMapsSettingsXMLDataModel;

	return true;
}


void BalancePanel::destroyDataModel()
{
	auto pDefinitionManager = this->get< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return;
	}
	auto & definitionManager = (*pDefinitionManager);

	auto definition = contextObject_.getDefinition( definitionManager );
	{
		PropertyAccessor accessor = definition->bindProperty( "mapsSettingsXMLData", contextObject_ );
		auto oldModel = contextObject_->pMapsSettingsXMLDataModel_;
		IListModel * nullModel = nullptr;
		ObjectHandle nullHandle = nullModel;

		auto & listeners = definitionManager.getPropertyAccessorListeners();
		auto itBegin = listeners.cbegin();
		auto itEnd = listeners.cend();

		for (auto it = itBegin; it != itEnd; ++it)
		{
			it->get()->preSetValue( accessor, nullHandle );
		}

		contextObject_->pMapsSettingsXMLDataModel_ = nullptr;

		for (auto it = itBegin; it != itEnd; ++it)
		{
			it->get()->postSetValue( accessor, nullHandle );
		}

		delete oldModel;
	}

	contextObject_ = nullptr;
}


bool BalancePanel::addPanel()
{
	auto pUIFramework = this->get< IUIFramework >();
	auto pUIApplication = this->get< IUIApplication >();
	
	if (pUIFramework == nullptr)
	{
		NGT_ERROR_MSG( "Failed to get IUIFramework\n" );
		return false;
	}
	auto & uiFramework = (*pUIFramework);

	if (pUIApplication == nullptr)
	{
		NGT_ERROR_MSG( "Failed to get IUIApplication\n" );
		return false;
	}
	auto & uiApplication = (*pUIApplication);

	pythonView_ = pUIFramework->createView(
		"MapsSettings/MapsSettingsPanel.qml",
		IUIFramework::ResourceType::Url, contextObject_ );

	pUIApplication->addView( *pythonView_ );
	return true;
}


void BalancePanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *pythonView_ );
	pythonView_ = nullptr;
}
