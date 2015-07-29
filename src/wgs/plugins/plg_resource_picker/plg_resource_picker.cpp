#include "generic_plugin/generic_plugin.hpp"
#include "qt_common/shared_controls.hpp"
#include <QWidget>

class AssetBrowserPlugin
	: public PluginMain
{
public:
	AssetBrowserPlugin(IComponentContext & contextManager){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		SharedControls::init();
		Q_INIT_RESOURCE(controls);
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
		Q_CLEANUP_RESOURCE(controls);
	}

private:
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC(AssetBrowserPlugin)

