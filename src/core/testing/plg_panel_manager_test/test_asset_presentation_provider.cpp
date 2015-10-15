#include "test_asset_presentation_provider.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include <fstream>

void TestAssetPresentationProvider::generateData()
{
	testThumbnails_.clear();

	addThumbnail( CACHED_DEFAULT, "plugins/test_resources/test_thumbnail_default.png" );
	addThumbnail( CACHED_SCRIPT, "plugins/test_resources/test_thumbnail_script.png" );
	addThumbnail( CACHED_XML, "plugins/test_resources/test_thumbnail_xml.png" );
	addThumbnail( CACHED_DECAL, "plugins/test_resources/test_thumbnail_decal.png" );

	testStatusIcon_ = getBinaryDataFromFile( "plugins/test_resources/test_status_readonly.png" );
}

ThumbnailData TestAssetPresentationProvider::getBinaryDataFromFile( const char * filename )
{
	if (filename == nullptr)
	{
		return nullptr;
	}

	size_t length = 0;
	std::ifstream input( filename, std::ifstream::binary | std::ios::in );
	if (input.is_open())
	{
		input.seekg( 0, std::ios_base::end );
		length = input.tellg();
		input.seekg( 0, std::ios_base::beg );
	}

	if (length == 0)
	{
		return nullptr;
	}
	
	char * buffer = new char[ length ];
	input.read( buffer, length );
	input.close();

	return std::make_shared< BinaryBlock >( buffer, length, false );
}

void TestAssetPresentationProvider::addThumbnail( int index, const char * filename )
{
	if (filename == nullptr)
	{
		return;
	}

	ThumbnailData binaryData = getBinaryDataFromFile( filename );
	if (binaryData == nullptr)
	{
		return;
	}

	testThumbnails_.insert( { index, binaryData } );
}

ThumbnailData TestAssetPresentationProvider::getThumbnail( const IAssetObjectItem * asset )
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char * extStart = strchr( asset->getAssetName(), '.' ) + 1;

	int type = CACHED_DEFAULT;

	if (strcmp( extStart, "txt" ) == 0 || strcmp( extStart, "dll" ) == 0 || strcmp( extStart, "pdb" ) == 0)
	{
		type = CACHED_SCRIPT;
	}
	else if (strcmp( extStart, "xml" ) == 0)
	{
		type = CACHED_XML;
	}
	else if (strcmp( extStart, "png" ) == 0 || strcmp( extStart, "bmp" ) == 0 || strcmp( extStart, "jpg" ) == 0)
	{
		type = CACHED_DECAL;
	}

	return testThumbnails_[ type ];
}

ThumbnailData TestAssetPresentationProvider::getStatusIconData( const IAssetObjectItem * asset )
{
	if (asset != nullptr && asset->isReadOnly())
	{
		return testStatusIcon_;
	}

	return nullptr;
}

const char* TestAssetPresentationProvider::getTypeIconResourceString( const IAssetObjectItem * asset ) const
{
	if (asset == nullptr || asset->getAssetName() == nullptr)
	{
		return nullptr;
	}

	// Assumes the use of the BaseAssetObjectItem, whose asset name includes the extension
	const char * extStart = strchr( asset->getAssetName(), '.' ) + 1;

	// Only testing a handful of file types to make sure the functionality works as intended
	if (strcmp( extStart, "txt" ) == 0 || strcmp( extStart, "dll" ) == 0 || strcmp( extStart, "pdb" ) == 0)
	{
		return "qrc:///filetypeicons/script_16x16";
	}
	else if (strcmp( extStart, "xml" ) == 0)
	{
		return "qrc:///filetypeicons/xml_16x16";
	}
	else if (strcmp( extStart, "png" ) == 0 || strcmp( extStart, "bmp" ) == 0 || strcmp( extStart, "jpg" ) == 0)
	{
		return "qrc:///filetypeicons/decal_16x16";
	}

	// Display default file icon
	return nullptr;
}
