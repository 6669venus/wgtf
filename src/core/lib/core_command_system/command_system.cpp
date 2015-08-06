#include "core_reflection/i_definition_manager.hpp"
#include "metadata/command_instance.mpp"
#include "metadata/compound_command.mpp"
#include "metadata/macro_object.mpp"

namespace CommandSystem
{

//==============================================================================
void initReflectedTypes( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( CommandInstance )
	REGISTER_DEFINITION( CompoundCommand )
	REGISTER_DEFINITION( CompoundCommandArgument )
	REGISTER_DEFINITION( MacroObject)
	REGISTER_DEFINITION( MacroEditObject)
}

}
