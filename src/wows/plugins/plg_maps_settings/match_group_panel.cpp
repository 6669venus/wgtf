#include "match_group_panel.hpp"
#include "match_group_list_model.hpp"

#include "core_data_model/collection_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"


MatchGroupPanel::MatchGroupPanel( IComponentContext & context,
	Collection & mapsSettingsXMLDataCollection,
	const char * title,
	const wchar_t * matchGroup )
	: Depends( context )
	, mapsSettingsXMLDataCollection_( mapsSettingsXMLDataCollection )
{
	this->createDataModel( context, title, matchGroup );
	this->addPanel();
}


MatchGroupPanel::~MatchGroupPanel()
{
	this->removePanel();
	this->destroyDataModel();
}


bool MatchGroupPanel::createDataModel( IComponentContext & context,
	const char * title,
	const wchar_t * matchGroup )
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
		matchGroup );

	contextObject_->title_ = title;

	return true;
}


void MatchGroupPanel::destroyDataModel()
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


bool MatchGroupPanel::addPanel()
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

	pvpPanelView_ = uiFramework.createView(
		"MapsSettings/MatchGroupPanel.qml",
		IUIFramework::ResourceType::Url,
		contextObject_ );
	uiApplication.addView( *pvpPanelView_ );

	return true;
}


void MatchGroupPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		NGT_ERROR_MSG( "Failed to get IUIApplication\n" );
		return;
	}
	uiApplication->removeView( *pvpPanelView_ );
}
