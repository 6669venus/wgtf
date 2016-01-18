#include "script_loader.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"


namespace PythonLoader
{


ObjectHandle createPythonObjects( IComponentContext & context )
{
	auto pDefinitionManager = context->queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	auto pScriptingEngine = context->queryInterface< IPythonScriptingEngine >();
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
	ObjectHandle rootPythonObject_;
	const bool isRootObject = rootObjectVariant.tryCast< ObjectHandle >( rootPythonObject_ );
	if (!isRootObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	auto rootObjectDefinition = rootPythonObject_.getDefinition( definitionManager );
	auto pXMLDataProperty = rootObjectDefinition->findProperty( "mapsSettingsXMLData" );
	if (pXMLDataProperty == nullptr)
	{
		NGT_ERROR_MSG( "Could not find property\n" );
		return false;
	}

	auto xmlDataVariant = pXMLDataProperty->get( rootPythonObject_, definitionManager );
	const bool isXMLDataObject =
		xmlDataVariant.tryCast< Collection >( mapsSettingsXMLDataCollection_ );
	if (!isXMLDataObject)
	{
		NGT_ERROR_MSG( "Could not get property\n" );
		return false;
	}

	return rootPythonObject_;
}


} // namespace PythonLoader

