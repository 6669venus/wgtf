#include "folder_content_object_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_data_model/i_item_role.hpp"

struct FolderContentObjectModel::Implementation
{
	Implementation( 
		FolderContentObjectModel& self, 
		const FileInfo& fileInfo );

	Implementation( FolderContentObjectModel& self );

	~Implementation()
	{
		delete fileInfo_;
	}

	FolderContentObjectModel& self_;
	FileInfo* fileInfo_;
};

FolderContentObjectModel::Implementation::Implementation(
	FolderContentObjectModel& self,
	const FileInfo& fileInfo )
	: self_( self )
	, fileInfo_( new FileInfo( fileInfo ) )
{
}

FolderContentObjectModel::Implementation::Implementation(
	FolderContentObjectModel& self )
	: self_( self )
	, fileInfo_( nullptr )
{
}

FolderContentObjectModel::FolderContentObjectModel()
	: impl_( new Implementation( *this ) )
{
}

FolderContentObjectModel::FolderContentObjectModel( 
	const FolderContentObjectModel& rhs )
	: impl_( new Implementation ( *this, *rhs.impl_->fileInfo_ ) )
{
}

FolderContentObjectModel::FolderContentObjectModel( const FileInfo& fileInfo )
	: impl_( new Implementation( *this, fileInfo ) )
{
}

FolderContentObjectModel::~FolderContentObjectModel()
{
}

void FolderContentObjectModel::init( const FileInfo& fileInfo )
{
	delete impl_->fileInfo_;
	impl_->fileInfo_ = new FileInfo( fileInfo );
}

const FileInfo& FolderContentObjectModel::getFileInfo() const
{
	assert( impl_->fileInfo_ );
	return *impl_->fileInfo_;
}

const char* FolderContentObjectModel::getFileName() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->name() : "";
}

const char* FolderContentObjectModel::getThumbnail() const
{
	return "";
}

const char* FolderContentObjectModel::getFullPath() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->fullPath.c_str() : "";
}

uint64_t FolderContentObjectModel::getSize() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->size : 0;
}

uint64_t FolderContentObjectModel::getCreatedTime() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->created : 0;
}

uint64_t FolderContentObjectModel::getModifiedTime() const
{
		return impl_->fileInfo_ ? impl_->fileInfo_->modified : 0;
}

uint64_t FolderContentObjectModel::getAccessedTime() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->accessed : 0;
}

bool FolderContentObjectModel::isDirectory() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->isDirectory() : false;
}

bool FolderContentObjectModel::isReadOnly() const
{
	return impl_->fileInfo_ ? impl_->fileInfo_->isReadOnly() : false;
}
