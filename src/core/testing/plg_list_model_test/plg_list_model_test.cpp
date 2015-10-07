#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

#include <vector>


//==============================================================================
class ListModelTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	ListModelTestPlugin(IComponentContext & contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{

	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{

	}

};


PLG_CALLBACK_FUNC( ListModelTestPlugin )

