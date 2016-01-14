#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "balance_panel.hpp"
#include "python_panel.hpp"
#include "map_status_panel.hpp"
#include "pvp_panel.hpp"
#include "pve_panel.hpp"
#include "pvp_ranked_panel.hpp"

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

		balancePanel_.reset( new BalancePanel( componentContext ) );

		pythonPanel_.reset( new PythonPanel( componentContext ) );
		pythonPanel_->initialize();

		mapStatusPanel_.reset( new MapStatusPanel( componentContext ) );
		mapStatusPanel_->addPanel();

		pvePanel_.reset( new PvePanel( componentContext ) );
		pvePanel_->addPanel();
		
		pvpPanel_.reset( new PvpPanel( componentContext ) );
		pvpPanel_->addPanel();

		pvpRankedPanel_.reset( new PvpRankedPanel( componentContext ) );
		pvpRankedPanel_->addPanel();
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pvpRankedPanel_->removePanel();
		pvpRankedPanel_.reset();
		pvpPanel_->removePanel();
		pvpPanel_.reset();
		pvePanel_->removePanel();
		pvePanel_.reset();
		mapStatusPanel_->removePanel();
		mapStatusPanel_.reset();
		pythonPanel_->finalize();
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
	std::unique_ptr<PvpPanel> pvpPanel_;
	std::unique_ptr<PvePanel> pvePanel_;
	std::unique_ptr<PvpRankedPanel> pvpRankedPanel_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
