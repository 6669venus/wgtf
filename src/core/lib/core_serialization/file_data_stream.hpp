
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_data_stream.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FILE_DATA_STREAM_H_
#define FILE_DATA_STREAM_H_

#pragma once

#include "core_serialization/i_datastream.hpp"
#include <fstream>

class FileDataStream : public IDataStream
{
public:
	FileDataStream( const char* path, std::ios::openmode mode = std::ios::in | std::ios::out );

	std::streamoff seek( std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg ) override;
	std::streamsize read( void* destination, std::streamsize size ) override;
	std::streamsize write( const void* source, std::streamsize size ) override;
	bool sync() override;

private:
	FileDataStream( const FileDataStream& );
	FileDataStream& operator=( const FileDataStream& );

	std::filebuf file_;
};

#endif // FILE_DATA_STREAM_H_
