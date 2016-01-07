#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "python_panel.hpp"
#include "map_status_panel.hpp"

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
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		// Initialise variant system; this is required for every plugin that uses Variant.
		auto metaTypeManager = componentContext.queryInterface<IMetaTypeManager>();
		Variant::setMetaTypeManager( metaTypeManager );
                pythonPanel_->initialize();
                mapStatusPanel_->addPanel();
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pythonPanel_->finalize();
                pythonPanel_.reset();

                mapStatusPanel_->removePanel();
		return true;
	}


	void Unload( IComponentContext& componentContext ) override
	{
	}


        std::unique_ptr<MapStatusPanel> mapStatusPanel_;
        std::unique_ptr<PythonPanel> pythonPanel_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
