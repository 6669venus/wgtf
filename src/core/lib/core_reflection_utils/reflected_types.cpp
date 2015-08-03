#include "reflected_types.hpp"
#include "commands/metadata/set_reflectedproperty_command.mpp" 
#include "core_reflection/i_definition_manager.hpp"


namespace Reflection_Utils
{

void initReflectedTypes( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( ReflectedPropertyCommandArgument );
}

}