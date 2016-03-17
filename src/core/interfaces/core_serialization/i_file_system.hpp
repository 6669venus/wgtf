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

#include "core_serialization/i_datastream.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_serialization/i_file_info.hpp"

#include <functional>
#include <memory>

class IFileSystem
{
public:
	typedef std::function<bool(IFileInfoPtr&&)> EnumerateCallback;
	typedef std::unique_ptr<IDataStream> IStreamPtr;

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
	virtual IFileInfoPtr	getFileInfo(const char* path) const = 0;
	virtual bool			move(const char* path, const char* new_path) = 0;
	virtual IStreamPtr		readFile(const char* path, std::ios::openmode mode) const = 0;
	virtual bool			writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) = 0;
};

class FileInfo : public IFileInfo
{
public:
	FileInfo()
		: size_(0)
		, created_(0)
		, modified_(0)
		, accessed_(0)
		, fullPath_("")
		, attributes_(FileAttributes::None)
	{
	}

	FileInfo(uint64_t size, uint64_t created, uint64_t modified, uint64_t accessed,
		const std::string& fullPath, FileAttributes::FileAttribute attributes)
		: size_(size)
		, created_(created)
		, modified_(modified)
		, accessed_(accessed)
		, fullPath_(fullPath)
		, attributes_(attributes)
	{
	}

	bool isDirectory() const override
	{
		return (attributes_ & FileAttributes::Directory) == FileAttributes::Directory;
	}

	bool isReadOnly() const override
	{
		return (attributes_ & FileAttributes::ReadOnly) == FileAttributes::ReadOnly;
	}

	bool isHidden() const override
	{
		return (attributes_ & FileAttributes::Hidden) == FileAttributes::Hidden;
	}

	bool isDots() const override
	{
		return isDirectory() && fullPath_.length() > 0 && fullPath_.back() == '.';
	}

	const char* name() const override
	{
		auto index = fullPath_.find_last_of("/\\");
		return &fullPath_.c_str()[index != std::string::npos ? index + 1 : 0];
	}

	const char* extension() const override
	{
		auto index = fullPath_.rfind( FilePath::kExtensionSeparator );
		return index != std::string::npos ? &fullPath_.c_str()[index + 1] : "";
	}

	const char* fullPath() const override
	{
		return fullPath_.c_str();
	}
	
	uint64_t size() const override { return size_; }
	uint64_t created() const override { return created_; }
	uint64_t modified() const override { return modified_; }
	uint64_t accessed() const override { return accessed_; }

	const FileAttributes::FileAttribute	attributes() const override { return attributes_; }

private:
	const uint64_t			size_;
	const uint64_t			created_;
	const uint64_t			modified_;
	const uint64_t			accessed_;
	const std::string		fullPath_;

	const FileAttributes::FileAttribute	attributes_;
};

#endif // I_FILE_SYSTEM_HPP_
