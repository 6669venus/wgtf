#include "progress_manager.hpp"

#include "generic_plugin/generic_plugin.hpp"


class ProgressManagerUIPlugin
	: public PluginMain
{
public:

	//==========================================================================
	ProgressManagerUIPlugin( IContextManager & contextManager )
	{
	}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IContextManager & contextManager )
	{
		progressManager_ = new ProgressManager();
		progressManager_->init( contextManager );
	}

	//==========================================================================
	bool Finalise( IContextManager & contextManager )
	{
		delete progressManager_;
		progressManager_ = nullptr;

		return true;
	}

private:
	ProgressManager * progressManager_;

};

PLG_CALLBACK_FUNC( ProgressManagerUIPlugin )
