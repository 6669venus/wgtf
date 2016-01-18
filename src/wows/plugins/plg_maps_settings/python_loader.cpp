#include "python_loader.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"


namespace PythonLoader
{


bool createPythonObjects( IComponentContext & context,
	ObjectHandle & outRootPythonObject,
	Collection & outMapsSettingsXMLDataCollection )
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

	if (!scriptingEngine.appendPath(
		L"..\\..\\..\\src\\wows\\plugins\\plg_maps_settings\\scripts" ))
	{
		NGT_ERROR_MSG( "Failed to append path\n" );
		return false;
	}

	ObjectHandle module = scriptingEngine.import( "MapsSettingsEditor" );
	if (!module.isValid())
	{
		NGT_ERROR_MSG( "Could not import module\n" );
		return false;
	}

	// MapsSettingsEditor.rootPythonObject
	auto moduleDefinition = module.getDefinition( definitionManager );
	auto pProperty = moduleDefinition->findProperty( "rootPythonObject" );
	if (pProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	auto rootObjectVariant = pProperty->get( module, definitionManager );
	const bool isRootObject = rootObjectVariant.tryCast< ObjectHandle >( outRootPythonObject );
	if (!isRootObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	auto rootObjectDefinition = outRootPythonObject.getDefinition( definitionManager );
	auto pXMLDataProperty = rootObjectDefinition->findProperty( "mapsSettingsXMLData" );
	if (pXMLDataProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	auto xmlDataVariant = pXMLDataProperty->get( outRootPythonObject, definitionManager );
	const bool isXMLDataObject =
		xmlDataVariant.tryCast< Collection >( outMapsSettingsXMLDataCollection );
	if (!isXMLDataObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	return true;
}


} // namespace PythonLoader

