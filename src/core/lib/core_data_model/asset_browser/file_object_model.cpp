#include "file_object_model.hpp"

struct FileObjectModel::Implementation
{
	Implementation( 
		FileObjectModel& self, 
		const FileInfo& fileInfo );

	Implementation( FileObjectModel& self );

	FileObjectModel& self_;
	FileInfo fileInfo_;
};

FileObjectModel::Implementation::Implementation(
	FileObjectModel& self,
	const FileInfo& fileInfo )
	: self_( self )
	, fileInfo_( fileInfo )
{
}

FileObjectModel::Implementation::Implementation(
	FileObjectModel& self )
	: self_( self )
{
}

FileObjectModel::FileObjectModel()
	: impl_( new Implementation( *this ) )
{
}

FileObjectModel::FileObjectModel( 
	const FileObjectModel& rhs )
	: impl_( new Implementation ( *this, rhs.impl_->fileInfo_ ) )
{
}

FileObjectModel::FileObjectModel( const FileInfo& fileInfo )
	: impl_( new Implementation( *this, fileInfo ) )
{
}

FileObjectModel::~FileObjectModel()
{
}

const FileInfo& FileObjectModel::getFileInfo() const
{
	return impl_->fileInfo_;
}

const char* FileObjectModel::getFileName() const
{
	return impl_->fileInfo_.name();
}

const char* FileObjectModel::getThumbnail() const
{
	return "";
}

const char* FileObjectModel::getFullPath() const
{
	return impl_->fileInfo_.fullPath.c_str();
}

uint64_t FileObjectModel::getSize() const
{
	return impl_->fileInfo_.size;
}

uint64_t FileObjectModel::getCreatedTime() const
{
	return impl_->fileInfo_.created;
}

uint64_t FileObjectModel::getModifiedTime() const
{
	return impl_->fileInfo_.modified;
}

uint64_t FileObjectModel::getAccessedTime() const
{
	return impl_->fileInfo_.accessed;
}

bool FileObjectModel::isDirectory() const
{
	return impl_->fileInfo_.isDirectory();
}

bool FileObjectModel::isReadOnly() const
{
	return impl_->fileInfo_.isReadOnly();
}

bool FileObjectModel::isCompressed() const
{
	return (impl_->fileInfo_.attributes & FileAttributes::Compressed) == FileAttributes::Compressed;
}
