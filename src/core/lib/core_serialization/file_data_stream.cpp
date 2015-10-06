//----------------------------------------------------------------------------
//
//  file_data_stream.cpp
//
//----------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//----------------------------------------------------------------------------

#include "file_data_stream.hpp"

FileDataStream::FileDataStream( const char* path, std::ios::openmode mode ):
	file_()
{
	file_.open( path, mode );
}

std::streamoff FileDataStream::seek( std::streamoff offset, std::ios_base::seekdir dir )
{
	return file_.pubseekoff( offset, dir, std::ios_base::in | std::ios_base::out );
}


std::streamsize FileDataStream::read( void* destination, std::streamsize size )
{
	return file_.sgetn( static_cast<char*>( destination ), size );
}


std::streamsize FileDataStream::write( const void* source, std::streamsize size )
{
	return file_.sputn( static_cast<const char*>( source ), size );
}


bool FileDataStream::sync()
{
	return file_.pubsync() == 0;
}

