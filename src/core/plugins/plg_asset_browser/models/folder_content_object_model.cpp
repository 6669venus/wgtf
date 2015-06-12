#include "folder_content_object_model.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"

struct FolderContentObjectModel::Implementation
{
	Implementation( 
		FolderContentObjectModel& self, 
		const char* fileName,
		const char* thumbnail);

	FolderContentObjectModel& self_;
	std::string fileName_;
	std::string thumbnail_;
};

FolderContentObjectModel::Implementation::Implementation(
	FolderContentObjectModel& self,
	const char* fileName,
	const char* thumbnail)
	: self_( self )
	, fileName_( fileName )
	, thumbnail_( thumbnail )
{
}

FolderContentObjectModel::FolderContentObjectModel()
	: impl_( new Implementation( *this, "", "" ) )
{
}

FolderContentObjectModel::FolderContentObjectModel( 
	const FolderContentObjectModel& rhs )
	: impl_( new Implementation ( 
					*this, 
					rhs.impl_->fileName_.c_str(), 
					rhs.impl_->thumbnail_.c_str() ) )
{
}

FolderContentObjectModel::FolderContentObjectModel( const char* fileName,
												    const char* thumbnail )
	: impl_( new Implementation( *this, fileName, thumbnail ) )
{
}

FolderContentObjectModel::~FolderContentObjectModel()
{
}

void FolderContentObjectModel::init( const char* fileName, 
									 const char* thumbnail )
{
	impl_->fileName_ = fileName;
	impl_->thumbnail_ = thumbnail;
}

const char* FolderContentObjectModel::getFileName() const
{
	return impl_->fileName_.c_str();
}

const char* FolderContentObjectModel::getThumbnail() const
{
	return impl_->thumbnail_.c_str();
}
