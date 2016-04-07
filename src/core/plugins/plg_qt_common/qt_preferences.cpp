#include "qt_preferences.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_variant/interfaces/i_meta_type_manager.hpp"

namespace {
	const char* s_preferenceFile = "generic_app_test_preferences.xml";
	const char* s_definitionFile = "generic_app_test_QtFramework_ContextDefinition.xml";
}

//------------------------------------------------------------------------------
QtPreferences::QtPreferences( IDefinitionManager & definitionManger, 
							  ISerializationManager & serializationManager, 
							  IFileSystem & fileSystem,
							  IMetaTypeManager & metaTypeManager )
	: definitionManager_( definitionManger )
	, serializationManager_( serializationManager )
	, fileSystem_( fileSystem )
	, metaTypeManager_( metaTypeManager )
{
	if (fileSystem_.exists( s_definitionFile ))
	{
		IFileSystem::IStreamPtr fileStream = 
			fileSystem_.readFile( s_definitionFile, std::ios::in | std::ios::binary );
		XMLSerializer serializer( *fileStream, definitionManger );
		definitionManager_.deserializeDefinitions( serializer );
	}
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{
	auto stream = fileSystem_.readFile( s_definitionFile, std::ios::out | std::ios::binary );
	if(stream)
	{
		XMLSerializer serializer( *stream, definitionManager_ );
		definitionManager_.serializeDefinitions( serializer );
	}
}

//------------------------------------------------------------------------------
GenericObjectPtr & QtPreferences::getPreference( const char * key )
{
	auto findIt = preferences_.find( key );
	if (findIt != preferences_.end())
	{
		return findIt->second;
	}
	auto preference = GenericObject::create( definitionManager_ );
	preferences_[ key ] = preference;
	return preferences_[ key ];
}

//------------------------------------------------------------------------------
void QtPreferences::savePrferences()
{
	auto stream = fileSystem_.readFile( s_preferenceFile, std::ios::out | std::ios::binary );
	if(stream)
	{
		XMLSerializer serializer( *stream, definitionManager_ );
		size_t size = preferences_.size();
		serializer.serialize( size );
		for( auto& preferenceIter : preferences_ )
		{
			serializer.serialize( preferenceIter.first );
			serializer.serialize( preferenceIter.second );
		}
	}
}

//------------------------------------------------------------------------------
void QtPreferences::loadPreferences()
{
	if ((!fileSystem_.exists( s_definitionFile )) || (!fileSystem_.exists( s_preferenceFile )))
	{
		return;
	}
	preferences_.clear();

	IFileSystem::IStreamPtr fileStream = 
		fileSystem_.readFile( s_preferenceFile, std::ios::in | std::ios::binary );
	XMLSerializer serializer( *fileStream, definitionManager_ );
	size_t count = 0;
	serializer.deserialize( count );
	for (size_t i = 0; i < count; i++)
	{
		std::string key;
		serializer.deserialize( key );

		const MetaType * metaType = 
			metaTypeManager_.findType( getClassIdentifier<ObjectHandle>() );
		Variant value( metaType );
		serializer.deserialize( value );
		GenericObjectPtr obj;
		bool isOk = value.tryCast( obj );
		assert( isOk );
		preferences_[ key ] = obj;
	}
}