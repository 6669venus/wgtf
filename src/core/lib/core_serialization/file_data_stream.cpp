//----------------------------------------------------------------------------
//
//  file_data_stream.cpp
//
//----------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//----------------------------------------------------------------------------

#include "file_data_stream.hpp"


FileDataStream::FileDataStream( const char* path, std::ios::openmode mode ):
	base( &file_ )
{
	file_.open( path, mode );
}


