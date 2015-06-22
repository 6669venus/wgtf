//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_file_system.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_FILE_SYSTEM_HPP_
#define I_FILE_SYSTEM_HPP_

#pragma once

#include "serialization/i_datastream.hpp"

#include <functional>
#include <memory>

struct FileInfo;

namespace FileAttributes
{
	enum FileAttribute
	{
		None = 0x00000000,
		ReadOnly = 0x00000001,
		Hidden = 0x00000002,
		System = 0x00000004,
		Directory = 0x00000010,
		Archive = 0x00000020,
		Device = 0x00000040,
		Normal = 0x00000080,
		Temporary = 0x00000100,
		SparseFile = 0x00000200,
		ReparsePoint = 0x00000400,
		Compressed = 0x00000800,
		Offline = 0x00001000,
		NotContentIndexed = 0x00002000,
		Encrypted = 0x00004000,
		IntegrityStream = 0x00008000,
		Virtual = 0x00010000,
		NoScrubData = 0x00020000,

		AppendOnly,
		//Compressed,
		NoDump,
		ExtentFormat,
		Immutable,
		DataJournaling,
		SecureDeletion,
		NoTailMerging,
		Undeletable,
		NoAtimeUpdates,
		SynchronousDirectory,
		Updates,
		SynchronousUpdates,
		TopOfDirectoryHierarchy
	};
};

class IFileSystem
{
public:
	typedef std::function<bool(FileInfo&&)> EnumerateCallback;
	typedef std::unique_ptr<IDataStream> istream_uptr;

	enum FileType
	{
		NotFound,
		Directory,
		File,
		Archive
	};

	virtual ~IFileSystem(){}
	virtual bool			copy(const char* path, const char* new_path) = 0;
	virtual bool			remove(const char* path) = 0;
	virtual bool			exists(const char* path) const = 0;
	virtual void			enumerate(const char* dir, EnumerateCallback callback) const = 0;
	virtual FileType		getFileType(const char* path) const = 0;
	virtual FileInfo		getFileInfo(const char* path) const = 0;
	virtual bool			move(const char* path, const char* new_path) = 0;
	virtual istream_uptr	readFile(const char* path, std::ios::openmode mode) const = 0;
	virtual bool			writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) = 0;
};

struct FileInfo
{
	static const char		kAltDirectorySeparator = '\\';
	static const char		kDirectorySeparator = '/';
	static const char		kExtensionSeparator = '.';
	static const char		kVolumeSeparator = ':';

	FileInfo(uint64_t size, uint64_t created, uint64_t modified, uint64_t accessed,
		const std::string& fullPath, FileAttributes::FileAttribute attributes)
		: size(size)
		, created(created)
		, modified(modified)
		, accessed(accessed)
		, fullPath(fullPath)
		, attributes(attributes)
	{
	}

	bool isDirectory() const
	{
		return (attributes & FileAttributes::Directory) == FileAttributes::Directory;
	}

	bool isReadOnly() const
	{
		return (attributes & FileAttributes::ReadOnly) == FileAttributes::ReadOnly;
	}

	bool isHidden() const
	{
		return (attributes & FileAttributes::Hidden) == FileAttributes::Hidden;
	}

	bool isDots() const
	{
		return isDirectory() && fullPath.length() > 0 && fullPath.back() == '.';
	}

	const char* name() const
	{
		auto index = fullPath.rfind( kAltDirectorySeparator );
		if (index == std::string::npos)
		{
			fullPath.rfind( kDirectorySeparator );
		}
		return &fullPath.c_str()[index != std::string::npos ? index + 1 : 0];
	}

	const char* extension() const
	{
		auto index = fullPath.rfind( kExtensionSeparator );
		return index != std::string::npos ? &fullPath.c_str()[index + 1] : "";
	}

	const uint64_t			size;
	const uint64_t			created;
	const uint64_t			modified;
	const uint64_t			accessed;
	const std::string		fullPath;

	const FileAttributes::FileAttribute	attributes;
};

#endif // I_FILE_SYSTEM_HPP_
