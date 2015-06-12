#include "copy_paste_manager.hpp"
#include "i_copyable_object.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "serialization/text_stream.hpp"


//TODO: Switch to multiplatform clipboard handles
#if defined( _WIN32 )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif // defined( _WIN32 )

namespace
{
	const char * s_ValueHintTag = "DATAHINT";
	const char * s_ValueTag = "COPYDATA";
}


//==============================================================================
CopyPasteManager::CopyPasteManager()
{
}


//==============================================================================
CopyPasteManager::~CopyPasteManager()
{
	
}


//==============================================================================
void CopyPasteManager::onSelect( ICopyableObject* pObject, bool append )
{
	assert( pObject != nullptr );
	if(!append)
	{
		curObjects_.clear();
	}
	curObjects_.push_back( pObject );
}


//==============================================================================
void CopyPasteManager::onDeselect( ICopyableObject* pObject, bool reset )
{
	if(reset)
	{
		curObjects_.clear();
		return;
	}
	curObjects_.erase( 
		std::remove( curObjects_.begin(), curObjects_.end(), pObject ), curObjects_.end() );
}


//==============================================================================
bool CopyPasteManager::copy()
{
	assert( !curObjects_.empty() );
	TextStream stream("", std::ios::out);
	bool ret = true;
	std::vector< ICopyableObject* >::iterator iter;
	for (iter = curObjects_.begin(); iter != curObjects_.end() && ret; ++iter)
	{
		assert( *iter != nullptr );
		const char * hint = (*iter)->getDataHint();
		const Variant & value = (*iter)->getData();
		if(strcmp(hint, "") != 0)
		{
			stream.write(s_ValueHintTag);
			stream.write( hint );
		}
		stream.write(s_ValueTag);
		ret = serializeData( stream, value );
	}
	if(!ret)
	{
		assert( false );
		return false;
	}
	if (!OpenClipboard( NULL ))
	{
		assert( false );
		return false;
	}

	// copy data to clipboard
	std::string data = stream.getData();
	size_t size = data.length();
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,size + 1);
	if (!hg){
		CloseClipboard();
		assert( false );
		return false;
	}
	memcpy( GlobalLock(hg),data.c_str(),size + 1 );
	GlobalUnlock(hg);
	HANDLE newHandle = SetClipboardData( CF_TEXT,hg );
	int errorcode = ::GetLastError();
	CloseClipboard();
	GlobalFree(hg);

	return ret;
}


//==============================================================================
bool CopyPasteManager::paste()
{
	assert( !curObjects_.empty() );

	if (!OpenClipboard( NULL )) 
	{
		assert( false );
		return false;
	}

	// get data from clipboard
	HANDLE hClipboardData = GetClipboardData(CF_TEXT);
	SIZE_T length = GlobalSize( hClipboardData );
	char * pData = (char*)GlobalLock(hClipboardData);
	assert( pData != nullptr );
	std::string str( pData, length );
	TextStream stream( str, std::ios::in );

	GlobalUnlock(hClipboardData);
	CloseClipboard();

	
	bool ret = true;
	std::vector< ICopyableObject* >::iterator iter;
	for (iter = curObjects_.begin(); iter != curObjects_.end() && ret; ++iter)
	{
		assert( *iter != nullptr );
		Variant value = (*iter)->getData();
		stream.seek(0);
		std::string tag;
		std::string hint;
		stream.read( tag );
		if(tag == s_ValueHintTag)
		{
			stream.read(hint);
			std::string valueTag;
			stream.read(valueTag);
			assert( valueTag == s_ValueTag );
		}
		else
		{
			assert( tag == s_ValueTag);
		}
		if (!deserializeData( stream, value ))
		{
			assert( false );
			continue;
		}
		ret = (*iter)->setData( value );
	}

	return ret;
}


//==============================================================================
bool CopyPasteManager::canCopy() const
{
	if (curObjects_.empty())
	{
		return false;
	}
	return true;
}


//==============================================================================
bool CopyPasteManager::canPaste() const
{
	return (IsClipboardFormatAvailable(CF_TEXT) != 0);
}


//==============================================================================
void CopyPasteManager::init( ISerializationManager * serializationMgr )
{
	curObjects_.clear();
	serializationMgr_ = serializationMgr;
}

//==============================================================================
void CopyPasteManager::fini()
{
	curObjects_.clear();
	serializationMgr_ = nullptr;
}


//==============================================================================
bool CopyPasteManager::serializeData( IDataStream& stream, const Variant & value )
{
	return serializationMgr_->serialize( stream, value );
}


//==============================================================================
bool CopyPasteManager::deserializeData( IDataStream& stream, Variant& value )
{
	return serializationMgr_->deserialize( stream, value );
}

