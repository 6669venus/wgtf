#include "test_datasource.hpp"
#include "pages/test_page.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "wg_types/binary_block.hpp"
#include "core_command_system/i_command_manager.hpp"
#include <fstream>


namespace {
	typedef XMLSerializer HistorySerializer;
	static const char * s_historyVersion = "ui_main_ver_1_0_12.";
	const std::string s_objectFile( "generic_app_test_" + std::string(s_historyVersion) );
	const std::string s_historyFile( "generic_app_test_cmd_history_"  + std::string(s_historyVersion) );
}

TestDataSource::TestDataSource()
	: testPageId_ ( "" )
	, testPageId2_( "" )
	, testPage_( nullptr )
	, testPage2_( nullptr )
{
	loadedObj_.clear();
}

TestDataSource::~TestDataSource()
{

}

void TestDataSource::init( IComponentContext & contextManager, int id )
{
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	if (defManager == NULL)
	{
		return;
	}
	auto objManager = contextManager.queryInterface< IObjectManager >();
	auto commandSysProvider = contextManager.queryInterface<ICommandManager>();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();

	std::string objectFile = s_objectFile + std::to_string(id);

	if (fileSystem && objManager && defManager)
	{
		if (fileSystem->exists( objectFile.c_str() ))
		{
			IFileSystem::istream_uptr fileStream = 
				fileSystem->readFile( objectFile.c_str(), std::ios::in | std::ios::binary );
			HistorySerializer serializer( *fileStream, *defManager );

			// read version
			std::string version;
			serializer.deserialize( version );
			if(version == s_historyVersion)
			{
				bool br = serializer.deserialize( testPageId_ );
				br = serializer.deserialize( testPageId2_ );
				// load objects
				objManager->registerListener( this );
				defManager->deserializeDefinitions( serializer );
				br = objManager->loadObjects( serializer );
				assert( br );
				objManager->deregisterListener( this );
			}
			loadedObj_.clear();
		}
		if (commandSysProvider != nullptr)
		{
			std::string historyFile = s_historyFile + std::to_string(id);
			if (fileSystem->exists( historyFile.c_str() ))
			{
				IFileSystem::istream_uptr fileStream = 
					fileSystem->readFile( historyFile.c_str(), std::ios::in | std::ios::binary );
				HistorySerializer serializer( *fileStream, *defManager );
				// read version
				std::string version;
				serializer.deserialize( version );
				if( version == s_historyVersion)
				{
					// read data
					commandSysProvider->LoadHistory( serializer );
				}
			}
		}
	}
	if (testPage_ == nullptr)
	{
		testPage_ = defManager->create< TestPage >();
		testPage_->init();
		RefObjectId id;
		bool ok = testPage_.getId( id );
		assert( ok );
		testPageId_ = id.toString();
	}
	if (testPage2_ == nullptr)
	{
		testPage2_ = defManager->create< TestPage2 >();
		testPage2_->init( *defManager );
		RefObjectId id;
		bool ok = testPage2_.getId( id );
		assert( ok );
		testPageId2_ = id.toString();
	}
}

void TestDataSource::fini( IComponentContext & contextManager, int id )
{
	auto objManager = contextManager.queryInterface< IObjectManager >();
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	auto commandSysProvider = contextManager.queryInterface<ICommandManager>();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();
	if (objManager && defManager && fileSystem)
	{
		// save objects data
		{
			ResizingMemoryStream stream;
			HistorySerializer serializer( stream, *defManager );
			// write version
			serializer.serialize( s_historyVersion );
			// save objects' ids which help to restore to the member when loading back
			serializer.serialize( testPageId_ );
			serializer.serialize( testPageId2_ );

			// save objects
			defManager->serializeDefinitions( serializer );
			bool br = objManager->saveObjects( *defManager, serializer );
			assert( br );

			std::string objectFile = s_objectFile + std::to_string(id);
			serializer.sync();
			fileSystem->writeFile( 
				objectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
		}

		// save command history
		if(commandSysProvider != nullptr)
		{
			ResizingMemoryStream stream;
			HistorySerializer serializer( stream, *defManager );
			// write version
			serializer.serialize( s_historyVersion );
			// save data
			commandSysProvider->SaveHistory( serializer );

			std::string historyFile = s_historyFile + std::to_string(id);
			fileSystem->writeFile( 
				historyFile.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
		}
	}
	else
	{
		assert( false );
	}
}

const ObjectHandleT< TestPage > & TestDataSource::getTestPage() const
{
	return testPage_;
}

const ObjectHandleT< TestPage2 > & TestDataSource::getTestPage2() const
{
	return testPage2_;
}

std::shared_ptr< BinaryBlock > TestDataSourceManager::getThumbnailImage()
{
	static std::unique_ptr< char[] > buffer;
	static int filesize = 0;	
	if (buffer == nullptr)
	{
#ifndef _WINGDI_ 
#pragma pack(push, 1)
		typedef struct tagBITMAPFILEHEADER 
		{
			unsigned short    bfType;        // must be 'BM' 
			unsigned long    bfSize;        // size of the whole .bmp file
			unsigned short     bfReserved1;   // must be 0
			unsigned short     bfReserved2;   // must be 0
			unsigned long    bfOffBits;     
		} BITMAPFILEHEADER; 

		typedef struct tagBITMAPINFOHEADER
		{
			unsigned long   biSize;            // size of the structure
			long   biWidth;           // image width
			long   biHeight;          // image height
			unsigned short    biPlanes;          // bitplanes
			unsigned short    biBitCount;         // resolution 
			unsigned long   biCompression;     // compression
			unsigned long   biSizeImage;       // size of the image
			long   biXPelsPerMeter;   // pixels per meter X
			long   biYPelsPerMeter;   // pixels per meter Y
			unsigned long  biClrUsed;         // colors used
			unsigned long   biClrImportant;    // important colors
		} BITMAPINFOHEADER;
#pragma pack(pop)
#endif
		char * tmp = nullptr;
		int headersize = sizeof (BITMAPFILEHEADER );
		int infosize = sizeof (BITMAPINFOHEADER );
		filesize = headersize + infosize + 64*64*3;
		buffer.reset(  new char[filesize] );
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER info;
		memset ( &bmfh, 0, headersize );
		memset ( &info, 0, infosize);
		bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfSize = headersize+ 
			infosize+ 0;
		bmfh.bfOffBits = 0x36;

		info.biSize = infosize;
		info.biWidth = 64;
		info.biHeight = 64;
		info.biPlanes = 1;	
		info.biBitCount = 24;
		info.biCompression = 0L;	
		info.biSizeImage = 0;
		info.biXPelsPerMeter = 0x0ec4;  
		info.biYPelsPerMeter = 0x0ec4;     
		info.biClrUsed = 0;	
		info.biClrImportant = 0; 
		memcpy(buffer.get(), &bmfh, headersize);
		memcpy(buffer.get() +headersize, &info, infosize);
		for (int i = 0; i < 64; i++)
		{
			tmp = (buffer.get() + headersize + infosize) + (i * 64 * 3);
			for (int j = 0; j< 64 * 3; j += 3)
			{
				tmp[j] = i*j % 256;
				tmp[j + 1] = i % 256;
				tmp[j + 2] = j % 256;
			}
		}
	}
	return std::make_shared< BinaryBlock >( buffer.get(), filesize, true );
}

void TestDataSource::onObjectRegistered(const ObjectHandle & pObj)
{
	RefObjectId id;
	bool ok = pObj.getId( id );
	assert( ok );
	if(id == testPageId_)
	{
		testPage_ = safeCast< TestPage >( pObj );
	}
	else if(id == testPageId2_)
	{
		testPage2_ = safeCast< TestPage2 >( pObj );
	}
	else
	{
		loadedObj_.insert( std::make_pair( id, pObj ) );
	}
}
void TestDataSource::onObjectDeregistered(const ObjectHandle & obj )
{
	RefObjectId id;
	bool ok = obj.getId( id );
	assert( ok );
	auto findIt = loadedObj_.find( id );
	if(findIt != loadedObj_.end())
	{
		loadedObj_.erase( findIt );
	}
}

void TestDataSourceManager::init(IComponentContext & contextManager)
{
	contextManager_ = &contextManager;
}

void TestDataSourceManager::fini()
{
	assert( contextManager_ );
	for (auto& p : sources_)
	{
		p.second->fini(*contextManager_, p.first);
	}

	sources_.resize(0);
	id_ = 0;
}

IDataSource* TestDataSourceManager::openDataSource()
{
	TestDataSource* ds = new TestDataSource;
	sources_.emplace_back( DataSources::value_type(id_, std::unique_ptr<TestDataSource>(ds)) );
	ds->init(*contextManager_, id_);
	++id_;
	return ds;
}

void TestDataSourceManager::closeDataSource(IDataSource* data)
{
	auto it = std::find_if( sources_.begin(), sources_.end(), 
		[=](const DataSources::value_type& p) { return p.second.get() == data; } );

	assert( it != sources_.end() );
	it->second->fini(*contextManager_, it->first);
	sources_.erase(it);
}
