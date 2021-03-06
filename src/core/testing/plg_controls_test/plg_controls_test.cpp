#include "core_generic_plugin/generic_plugin.hpp"

#include "controls_test_panel.hpp"

#include "core_variant/variant.hpp"


#include <memory>


namespace wgt
{
class ControlsTestUIPlugin
	: public PluginMain
{
public:
	ControlsTestUIPlugin( IComponentContext & componentContext )
	{
	}


	bool PostLoad( IComponentContext & componentContext ) override
	{
		Variant::setMetaTypeManager(
			componentContext.queryInterface< IMetaTypeManager >() );
		controlsTestPanel_.reset( new ControlsTestPanel( componentContext ) );
		return true;
	}


	void Initialise( IComponentContext & componentContext ) override
	{
		controlsTestPanel_->addPanel();
	}


	bool Finalise( IComponentContext & componentContext ) override
	{
		controlsTestPanel_->removePanel();
		return true;
	}


	void Unload( IComponentContext & componentContext ) override
	{
		controlsTestPanel_.reset();
	}

private:
	std::unique_ptr< ControlsTestPanel > controlsTestPanel_;
};


PLG_CALLBACK_FUNC( ControlsTestUIPlugin )
} // end namespace wgt
