#include "qt_preferences.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_variant/interfaces/i_meta_type_manager.hpp"

namespace {
	const char* s_preferenceFile = "generic_app_test_preferences.txt";
	const char* s_definitionFile = "generic_app_test_QtFramework_ContextDefinition.txt";
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
		IFileSystem::istream_uptr fileStream = 
			fileSystem_.readFile( s_definitionFile, std::ios::in | std::ios::binary );
		size_t size = fileStream->size();
		char * data = new char[size];
		fileStream->readRaw( data, size );
		fileStream = nullptr;
		TextStream stream( data, std::ios::in );
		definitionManager_.deserializeDefinitions( stream );
		delete[] data;
		data = nullptr;
	}
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{
	TextStream stream("", std::ios::out);
	definitionManager_.serializeDefinitions( stream );
	const std::stringbuf * pbuf = static_cast< const std::stringbuf * >( stream.rawBuffer() );
	fileSystem_.writeFile( 
		s_definitionFile, pbuf->str().c_str(), stream.size(), std::ios::out | std::ios::binary );
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
	TextStream stream("", std::ios::out);
	size_t size = preferences_.size();
	stream.write( size );
	for( auto& preferenceIter : preferences_ )
	{
		stream.write( preferenceIter.first );
		serializationManager_.serialize( stream, preferenceIter.second );
	}
	const std::stringbuf * pbuf = static_cast< const std::stringbuf * >( stream.rawBuffer() );
	fileSystem_.writeFile( 
		s_preferenceFile, pbuf->str().c_str(), stream.size(), std::ios::out | std::ios::binary );
}

//------------------------------------------------------------------------------
void QtPreferences::loadPreferences()
{
	if ((!fileSystem_.exists( s_definitionFile )) || (!fileSystem_.exists( s_preferenceFile )))
	{
		return;
	}
	preferences_.clear();

	IFileSystem::istream_uptr fileStream = 
		fileSystem_.readFile( s_preferenceFile, std::ios::in | std::ios::binary );
	size_t size = fileStream->size();
	char * data = new char[size];
	fileStream->readRaw( data, size );
	fileStream = nullptr;
	TextStream stream( data, std::ios::in );
	size_t count = 0;
	stream.read( count );
	for (size_t i = 0; i < count; i++)
	{
		std::string key;
		stream.read( key );

		const MetaType * metaType = 
			metaTypeManager_.findType( getClassIdentifier<ObjectHandle>() );
		Variant value( metaType );
		serializationManager_.deserialize( stream, value );
		GenericObjectPtr obj;
		bool isOk = value.tryCast( obj );
		assert( isOk );
		preferences_[ key ] = obj;
	}

	delete[] data;
	data = nullptr;
}