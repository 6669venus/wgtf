//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_system.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#pragma once

#include "serialization/interfaces/i_file_system.hpp"

class FileSystem : public Implements < IFileSystem >
{
public:
	virtual bool copy(const char* path, const char* new_path) override;
	virtual bool remove(const char* path) override;
	virtual bool exists(const char* path) const override;
	virtual void enumerate(const char* dir, EnumerateCallback callback) const override;
	virtual FileType getFileType(const char* path) const override;
	virtual FileInfo getFileInfo(const char* path) const override;
	virtual bool move(const char* path, const char* new_path) override;
	virtual istream_uptr readFile(const char* path, std::ios::openmode mode) const override;
	virtual bool writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) override;
};

#endif // FILE_SYSTEM_H_