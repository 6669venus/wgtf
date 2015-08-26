//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "core_common/ngt_windows.hpp"
#include "core_serialization/file_system.hpp"
#include "core_serialization/file_data_stream.hpp"

using namespace FileAttributes;

bool FileSystem::copy(const char* path, const char* new_path)
{
	return false;
}

bool FileSystem::remove(const char* path)
{
	return false;
}

bool FileSystem::exists(const char* path) const
{
	return false;
}
void FileSystem::enumerate(const char* dir, EnumerateCallback callback) const
{
}

IFileSystem::FileType FileSystem::getFileType(const char* path) const
{
	return IFileSystem::NotFound;
}

FileInfo FileSystem::getFileInfo(const char* path) const
{
	FileInfo info( 0, 0, 0, 0, std::string(), None );
	return info;
}

bool FileSystem::move(const char* path, const char* new_path)
{
	return MoveFileA(path, new_path) != FALSE;
}

IFileSystem::istream_uptr FileSystem::readFile(const char* path, std::ios::openmode mode) const
{
	return istream_uptr(new FileDataStream(path, mode));
}

bool FileSystem::writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode)
{
	return false;
}
