#include "folder_guard.hpp"
#include "ngt_core_common/ngt_windows.hpp"

#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

//==============================================================================
FolderSetter::FolderSetter()
{
	GetCurrentDirectory( ARRAY_SIZE( envFolder_ ), envFolder_ );
	GetCurrentDirectory( ARRAY_SIZE( curFolder_ ), curFolder_ );
}


//==============================================================================
void FolderSetter::enter()
{
	GetCurrentDirectory( ARRAY_SIZE( envFolder_ ), envFolder_ );
	SetCurrentDirectory( curFolder_ );
}


//==============================================================================
void FolderSetter::leave()
{
	GetCurrentDirectory( ARRAY_SIZE( curFolder_ ), curFolder_ );
	SetCurrentDirectory( envFolder_ );
}


//==============================================================================
FolderGuard::FolderGuard()
{
	setter_.enter();
}


//==============================================================================
FolderGuard::~FolderGuard()
{
	setter_.leave();
}
 