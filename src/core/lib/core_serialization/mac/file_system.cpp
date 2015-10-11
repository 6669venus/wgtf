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

#include <stdio.h>
#include <stdlib.h>
#include <copyfile.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace FileAttributes;

namespace
{

FileInfo CreateFileInfo()
{
    return FileInfo(0, 0, 0, 0, std::string(), None);
}

FileInfo CreateFileInfo(struct stat & fileStat, std::string && fullPath)
{
    unsigned int attributes = FileAttributes::None;
    
    if (S_ISDIR(fileStat.st_mode))
        attributes |= FileAttributes::Directory;
    
    if (S_ISREG(fileStat.st_mode))
        attributes |= FileAttribute::Normal;
    
    if (access(fullPath.c_str(), W_OK) != 0)
        attributes |= FileAttribute::ReadOnly;
    
    size_t separator = fullPath.rfind(FilePath::kDirectorySeparator);
    if (separator != std::string::npos && fullPath[separator + 1] == '.')
        attributes |= FileAttribute::Hidden;
    
    return FileInfo(fileStat.st_size, fileStat.st_mtimespec.tv_sec,
                    fileStat.st_mtimespec.tv_sec, fileStat.st_atimespec.tv_sec,
                    std::move(fullPath), static_cast<FileAttribute>(attributes));
}
    
} // namespace

bool FileSystem::copy(const char* path, const char* new_path)
{
    return copyfile(path, new_path, nullptr, 0) == 0;
}

bool FileSystem::remove(const char* path)
{
    return ::remove(path) == 0;
}

bool FileSystem::exists(const char* path) const
{
    struct stat fileStat;
    return stat(path, &fileStat) == 0;
}

void FileSystem::enumerate(const char* dir, EnumerateCallback callback) const
{
    char pathBuffer[MAX_PATH];
    realpath(dir, pathBuffer);
    std::string dirPath(pathBuffer);
    
    DIR * directory = opendir(dir);
    struct dirent * entry = nullptr;
    while((entry = readdir(directory)) != nullptr)
    {
        std::string filePath = dirPath;
        filePath.reserve(filePath.size() + strlen(entry->d_name) + 1);
        filePath.append(1, FilePath::kDirectorySeparator).append(entry->d_name);

        struct stat fileStat;
        if (stat(filePath.c_str(), &fileStat) == 0)
        {
            if (callback(CreateFileInfo(fileStat, std::move(filePath))) == false)
                break;
        }
    }
}

IFileSystem::FileType FileSystem::getFileType(const char* path) const
{
    struct stat fileStat;
    if (stat(path, &fileStat) < 0)
        return IFileSystem::NotFound;
    
    if (S_ISDIR(fileStat.st_mode))
        return IFileSystem::Directory;
        
	return IFileSystem::File;
}

FileInfo FileSystem::getFileInfo(const char* path) const
{
    struct stat fileStat;
    if (stat(path, &fileStat) < 0)
        return CreateFileInfo();
    
    char pathBuffer[MAX_PATH];
    realpath(path, pathBuffer);
    
    return CreateFileInfo(fileStat, std::string(pathBuffer));
}

bool FileSystem::move(const char* path, const char* new_path)
{
    return ::rename(path, new_path) == 0;
}

IFileSystem::istream_uptr FileSystem::readFile(const char* path, std::ios::openmode mode) const
{
	return istream_uptr(new FileDataStream(path, mode));
}

bool FileSystem::writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode)
{
    std::fstream stream(path, mode);
    if (!stream.bad())
    {
        stream.write(reinterpret_cast<const char *>(data), len);
        stream.close();
        return true;
    }
	return false;
}
