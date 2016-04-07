
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

#include "serialization_dll.hpp"

#include "std_data_stream.hpp"
#include <fstream>

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class SERIALIZATION_DLL FileDataStream:
	public StdDataStream
{
	typedef StdDataStream base;

public:
	FileDataStream( const char* path, std::ios::openmode mode = std::ios::in | std::ios::out );

private:
	FileDataStream( const FileDataStream& );
	FileDataStream& operator=( const FileDataStream& );

	std::filebuf file_;

};

#pragma warning (pop)

#endif // FILE_DATA_STREAM_H_