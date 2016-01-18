#include "pve_panel.hpp"
#include "match_group_list_model.hpp"

#include "core_data_model/collection_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"


PvePanel::PvePanel( IComponentContext & context )
	: Depends( context )
{
	this->createPythonObjects();
	this->createDataModel( context );
	this->addPanel();
}


PvePanel::~PvePanel()
{
	this->removePanel();
	this->destroyDataModel();
	this->destroyPythonObjects();
}


bool PvePanel::createPythonObjects()
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


void PvePanel::destroyPythonObjects()
{
	mapsSettingsXMLDataCollection_ = Collection();
	rootPythonObject_ = nullptr;
}


bool PvePanel::createDataModel( IComponentContext & context )
{
	auto pDefinitionManager = this->get< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	const bool managed = true;
	contextObject_ = definitionManager.create< PanelContext >( managed );

	// Construct an IListModel from the scripts
	contextObject_->pMapsSettingsXMLDataModel_ = new MatchGroupListModel(
		context,
		mapsSettingsXMLDataCollection_,
		L"pve" );

	return true;
}


void PvePanel::destroyDataModel()
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


bool PvePanel::addPanel()
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

	PvePanelView_ = uiFramework.createView(
		"MapsSettings/PvePanel.qml",
		IUIFramework::ResourceType::Url,
		contextObject_ );
	uiApplication.addView( *PvePanelView_ );

	return true;
}


void PvePanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		NGT_ERROR_MSG( "Failed to get IUIApplication\n" );
		return;
	}
	uiApplication->removeView( *PvePanelView_ );
}
