#include "generic_plugin/generic_plugin.hpp"
#include "qt_common/shared_controls.hpp"
#include <QWidget>

class AssetBrowserPlugin
	: public PluginMain
{
public:
	AssetBrowserPlugin(IContextManager & contextManager){}

	bool PostLoad( IContextManager & contextManager ) override
	{
		SharedControls::init();
		Q_INIT_RESOURCE(controls);
		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		return true;
	}

	void Unload( IContextManager & contextManager ) override
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

