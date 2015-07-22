
#include "game_common.h"

#include "game_loader.h"
#include "game_filesystem.h"

#include "serialization/interfaces/i_file_system.hpp"

#include <dsIO/FileInfo.h>
#include <dsIO/IDir.h>

#include <sstream>

GameFileSystem::GameFileSystem(const GameBinaryLoader& loader)
	: loader_(loader)
{
}

bool GameFileSystem::copy(const char* path, const char* new_path)
{
	// Not sure if/how this should be implemented
	throw std::logic_error("The method or operation is not implemented.");
	return false;
}

bool GameFileSystem::remove(const char* path)
{
	assert(!"TODO: For OS paths delete them, for DB paths verify the files were removed");

	Despair::PathSet paths;
	paths.emplace(path);
	loader_.GetFileMgr().RemoveFiles(paths);
	return true;
}
bool GameFileSystem::exists(const char* path) const
{
	return loader_.GetFileMgr().DirExists(path) || loader_.GetFileMgr().FileExists(path);
}

static FileInfo convertInfo(const Despair::FileInfo &despairInfo)
{
	int attributes = FileAttributes::None;
	if (despairInfo.m_stats.m_isLoose)
	{
	}
	else
	{
	}
	return FileInfo(
		uint64_t(despairInfo.m_stats.m_size),
		despairInfo.m_stats.m_createDate,
		despairInfo.m_stats.m_modifiedDate,
		despairInfo.m_stats.m_modifiedDate,
		despairInfo.m_dbPath.c_str(),
		(FileAttributes::FileAttribute)attributes
		);
}

static FileInfo convertInfo(const Despair::IDir& subDir)
{
	int attributes = FileAttributes::Directory;
	attributes |= subDir.IsHidden() * FileAttributes::Hidden;
	attributes |= subDir.IsLocal() * FileAttributes::Normal;
	attributes |= !subDir.IsLocal() * FileAttributes::Archive;
	return FileInfo(
		0,
		0,
		0,
		0,
		subDir.GetPath(),
		(FileAttributes::FileAttribute)attributes
		);
}

void GameFileSystem::enumerate(const char* dir, EnumerateCallback callback) const
{
	auto iDir = loader_.GetFileMgr().GetDir(dir);
	if (iDir)
	{
		auto dirs = iDir->GetSubDirs();
		for (auto subDir : dirs)
		{
			if (!callback(std::move(convertInfo(*subDir))))
				return;
		}
		Despair::FileInfoMap infos;
		iDir->GetFileInfos(infos);
		for (auto info : infos)
		{
			if (!callback(convertInfo(info.second)))
				return;
		}
	}
}

IFileSystem::FileType GameFileSystem::getFileType(const char* path) const
{
	// TODO: If the directory or file is in a resource pack return Archive?
	return loader_.GetFileMgr().DirExists(path) ? IFileSystem::Directory :
		loader_.GetFileMgr().FileExists(path) ? IFileSystem::File : IFileSystem::NotFound;
}

FileInfo GameFileSystem::getFileInfo(const char* path) const
{
	auto iDir = loader_.GetFileMgr().GetDir(path);
	if (iDir)
		return convertInfo(*iDir);
	Despair::FileInfo result;
	if (loader_.GetFileMgr().GetFileInfo(path, result) == Despair::DSR_OK)
	{
		return convertInfo(result);
	}
	return FileInfo(0, 0, 0, 0, "", FileAttributes::None);
}

bool GameFileSystem::move(const char* path, const char* new_path)
{
	throw std::logic_error("The method or operation is not implemented.");
	return false;
}

class DespairDataStream : public IDataStream
{
public:
	DespairDataStream(Despair::StreamPtr stream) : m_stream(stream){}

	virtual void seek(size_t pos) override
	{
		m_stream->Seek(static_cast<Despair::uint>(pos));
	}

	virtual size_t pos() const override
	{
		return m_stream->GetPosition();
	}

	virtual size_t size() const override
	{
		return m_stream->GetSize();
	}

	virtual const void * rawBuffer() const override
	{
		return m_stream->GetBuffer();
	}

	virtual size_t readRaw(void * o_Data, size_t length) override
	{
		return m_stream->ReadRaw(reinterpret_cast<Despair::byte*>(o_Data), static_cast<Despair::uint>(length));
	}

	virtual size_t writeRaw(const void * data, size_t length) override
	{
		return m_stream->WriteRaw(reinterpret_cast<const Despair::byte*>(data), static_cast<Despair::uint>(length));
	}

	virtual bool writeValue(const Variant & variant) override
	{
		std::stringstream stream;
		stream << variant;
		return writeRaw(stream.rdbuf(), stream.tellp()) != 0;
	}

	virtual bool readValue(Variant & variant) override
	{
		std::stringstream stream;
		auto pos = m_stream->GetPosition();
		stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(const_cast<Despair::byte *>(&m_stream->GetBuffer()[pos])),
			m_stream->GetBufferSize() - pos);
		stream >> variant;
		m_stream->Seek(static_cast<int>(stream.tellp()), Despair::Stream::STREAM_CURRENT);
		return m_stream->GetPosition() != pos;
	}
private:
	Despair::StreamPtr	m_stream;
};

IFileSystem::istream_uptr GameFileSystem::readFile(const char* path, std::ios::openmode mode) const
{
	return std::unique_ptr<IDataStream>(new DespairDataStream(loader_.GetFileMgr().OpenRead(path)));
}
bool GameFileSystem::writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode)
{
	throw std::logic_error("The method or operation is not implemented.");
	return false;
}
