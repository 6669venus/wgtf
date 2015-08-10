#include "file_object_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_data_model/i_item_role.hpp"

struct FileObjectModel::Implementation
{
	Implementation( 
		FileObjectModel& self, 
		const FileInfo& fileInfo );

	Implementation( FileObjectModel& self );

	~Implementation()
	{
		delete fileInfo_;
	}

	FileObjectModel& self_;
	FileInfo* fileInfo_;
};

FileObjectModel::Implementation::Implementation(
	FileObjectModel& self,
	const FileInfo& fileInfo )
	: self_( self )
	, fileInfo_( new FileInfo( fileInfo ) )
{
}

FileObjectModel::Implementation::Implementation(
	FileObjectModel& self )
	: self_( self )
	, fileInfo_( nullptr )
{
}

FileObjectModel::FileObjectModel()
	: impl_( new Implementation( *this ) )
{
}

FileObjectModel::FileObjectModel( 
	const FileObjectModel& rhs )
	: impl_( new Implementation ( *this, *rhs.impl_->fileInfo_ ) )
{
}

FileObjectModel::FileObjectModel( const FileInfo& fileInfo )
	: impl_( new Implementation( *this, fileInfo ) )
{
}

FileObjectModel::~FileObjectModel()
{
}

void FileObjectModel::init( const FileInfo& fileInfo )
{
	delete impl_->fileInfo_;
	impl_->fileInfo_ = new FileInfo( fileInfo );
}

const FileInfo& FileObjectModel::getFileInfo() const
{
	assert( impl_->fileInfo_ );
	return *impl_->fileInfo_;
}

const char* FileObjectModel::getFileName() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->name() : "";
}

const char* FileObjectModel::getThumbnail() const
{
	return "";
}

const char* FileObjectModel::getFullPath() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->fullPath.c_str() : "";
}

uint64_t FileObjectModel::getSize() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->size : 0;
}

uint64_t FileObjectModel::getCreatedTime() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->created : 0;
}

uint64_t FileObjectModel::getModifiedTime() const
{
		return impl_->fileInfo_ ? impl_->fileInfo_->modified : 0;
}

uint64_t FileObjectModel::getAccessedTime() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->accessed : 0;
}

bool FileObjectModel::isDirectory() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->isDirectory() : false;
}

bool FileObjectModel::isReadOnly() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->isReadOnly() : false;
}
