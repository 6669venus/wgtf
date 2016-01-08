#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
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
		pythonPanel_.reset( new PythonPanel( componentContext ) );
                mapStatusPanel_.reset( new MapStatusPanel( componentContext ) );
                pvpPanel_.reset( new PvpPanel( componentContext ) );
                pvePanel_.reset( new PvePanel( componentContext ) );
                pvpRankedPanel_.reset( new PvpRankedPanel( componentContext ) );
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		// Initialise variant system; this is required for every plugin that uses Variant.
		auto metaTypeManager = componentContext.queryInterface<IMetaTypeManager>();
		Variant::setMetaTypeManager( metaTypeManager );
                pythonPanel_->initialize();
                mapStatusPanel_->addPanel();
                pvpPanel_->addPanel();
                pvePanel_->addPanel();
                pvpRankedPanel_->addPanel();
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pythonPanel_->finalize();
                pythonPanel_.reset();

                mapStatusPanel_->removePanel();
                pvpPanel_->removePanel();
                pvePanel_->removePanel();
                pvpRankedPanel_->removePanel();
		return true;
	}


	void Unload( IComponentContext& componentContext ) override
	{
	}


        std::unique_ptr<MapStatusPanel> mapStatusPanel_;
        std::unique_ptr<PythonPanel> pythonPanel_;
        std::unique_ptr<PvpPanel> pvpPanel_;
        std::unique_ptr<PvePanel> pvePanel_;
        std::unique_ptr<PvpRankedPanel> pvpRankedPanel_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
