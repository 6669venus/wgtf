#ifndef GAME_FILESYSTEM_H
#define GAME_FILESYSTEM_H

#include "serialization/interfaces/i_file_system.hpp"

class GameBinaryLoader; 
class GameFileSystem : public Implements < IFileSystem >
{
public:

	GameFileSystem(const GameBinaryLoader& loader);

	virtual bool copy(const char* path, const char* new_path) override;
	virtual bool remove(const char* path) override;
	virtual bool exists(const char* path) const override;
	virtual void enumerate(const char* dir, EnumerateCallback callback) const override;
	virtual FileType getFileType(const char* path) const override;
	virtual FileInfo getFileInfo(const char* path) const override;
	virtual bool move(const char* path, const char* new_path) override;
	virtual istream_uptr readFile(const char* path, std::ios::openmode mode) const override;
	virtual bool writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) override;

private: 
	const GameBinaryLoader& loader_;
};

#endif // GAME_FILESYSTEM_H