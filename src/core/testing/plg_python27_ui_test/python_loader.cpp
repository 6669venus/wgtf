#include "python_loader.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"


namespace PythonLoader
{


bool createPythonObjects( IComponentContext & context,
	const wchar_t * path,
	const char * moduleName,
	ObjectHandle & module )
{
	auto pDefinitionManager = context.queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	auto pScriptingEngine = context.queryInterface< IPythonScriptingEngine >();
	if (pScriptingEngine == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IPythonScriptingEngine\n" );
		return false;
	}
	auto & scriptingEngine = (*pScriptingEngine);

	if (!scriptingEngine.appendPath( path ))
	{
		NGT_ERROR_MSG( "Failed to append path\n" );
		return false;
	}

	module = scriptingEngine.import( moduleName );
	if (!module.isValid())
	{
		NGT_ERROR_MSG( "Could not import module\n" );
		return false;
	}

	return true;
}


} // namespace PythonLoader

