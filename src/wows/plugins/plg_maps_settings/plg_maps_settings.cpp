#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "balance_panel.hpp"
#include "python_loader.hpp"
#include "python_panel.hpp"
#include "map_status_panel.hpp"
#include "match_group_panel.hpp"

#include "metadata/panel_context.mpp"

#include <memory>

struct Python27TestUIPlugin
		: public PluginMain
{
	Python27TestUIPlugin( IComponentContext& componentContext )
	{
	}


	bool PostLoad( IComponentContext& componentContext ) override
	{
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		// Initialise variant system; this is required for every plugin that uses Variant.
		auto metaTypeManager = componentContext.queryInterface<IMetaTypeManager>();
		Variant::setMetaTypeManager( metaTypeManager );

		auto pDefinitionManager = componentContext.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
			return;
		}
		auto & definitionManager = (*pDefinitionManager);

		REGISTER_DEFINITION( PanelContext )

		ObjectHandle rootPythonObject;
		Collection mapsSettingsXMLDataCollection;
		const auto loaded = PythonLoader::createPythonObjects( componentContext,
			rootPythonObject,
			mapsSettingsXMLDataCollection );
		if (!loaded)
		{
			NGT_ERROR_MSG( "Could not load from scripts\n" );
			return;
		}

		balancePanel_.reset( new BalancePanel( componentContext,
			mapsSettingsXMLDataCollection ) );

		pythonPanel_.reset( new PythonPanel( componentContext,
			rootPythonObject ) );

		mapStatusPanel_.reset( new MapStatusPanel( componentContext ) );
		mapStatusPanel_->addPanel();

		pvePanel_.reset( new MatchGroupPanel( componentContext,
			mapsSettingsXMLDataCollection,
			"PvE Settings",
			L"pve" ) );
		
		pvpPanel_.reset( new MatchGroupPanel( componentContext,
			mapsSettingsXMLDataCollection,
			"PvP Settings",
			L"pvp" ) );

		pvpRankedPanel_.reset( new MatchGroupPanel( componentContext,
			mapsSettingsXMLDataCollection,
			"PvP Ranked Settings",
			L"ranked" ) );
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pvpRankedPanel_.reset();
		pvpPanel_.reset();
		pvePanel_.reset();
		mapStatusPanel_->removePanel();
		mapStatusPanel_.reset();
		pythonPanel_.reset();
		balancePanel_.reset();

		return true;
	}


	void Unload( IComponentContext& componentContext ) override
	{
	}


	std::unique_ptr< BalancePanel > balancePanel_;
	std::unique_ptr< PythonPanel > pythonPanel_;
	std::unique_ptr<MapStatusPanel> mapStatusPanel_;
	std::unique_ptr< MatchGroupPanel > pvpPanel_;
	std::unique_ptr< MatchGroupPanel > pvePanel_;
	std::unique_ptr< MatchGroupPanel > pvpRankedPanel_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
