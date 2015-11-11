#include "core_generic_plugin/generic_plugin.hpp"
#include "python_panel.hpp"

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
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		pythonPanel_->initialise();
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pythonPanel_->finalise();
		return true;
	}


	void Unload( IComponentContext& componentContext )
	{
		pythonPanel_.reset();
	}


	std::unique_ptr<PythonPanel> pythonPanel_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
