#include "qt_copy_paste_manager.hpp"
#include "core_copy_paste/i_copyable_object.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_variant/collection.hpp"

#include <QtGui/QClipboard>
#include <QApplication>

namespace
{
	const char * s_ValueHintTag = "DATAHINT";
	const char * s_ValueTag = "COPYDATA";
}


//==============================================================================
QtCopyPasteManager::QtCopyPasteManager()
    : clipboard_( QApplication::clipboard() )
    , serializationMgr_( nullptr )
    , commandSystem_( nullptr )
{
}


//==============================================================================
QtCopyPasteManager::~QtCopyPasteManager()
{
	
}


//==============================================================================
void QtCopyPasteManager::onSelect( ICopyableObject* pObject, bool append )
{
	assert( pObject != nullptr );
	if(!append)
	{
		curObjects_.clear();
	}
	curObjects_.push_back( pObject );
}


//==============================================================================
void QtCopyPasteManager::onDeselect( ICopyableObject* pObject, bool reset )
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
bool QtCopyPasteManager::copy()
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
		if (strcmp(hint, "") != 0)
		{
			stream.write(s_ValueHintTag);
			stream.write( hint );
		}
		ret = serializeData( stream, value );
	}
	if (!ret)
	{
		assert( false );
		return false;
	}

	// copy data to clipboard
	std::string data = stream.getData();
	clipboard_->setText( QString::fromStdString( data ) );

	return ret;
}


//==============================================================================
bool QtCopyPasteManager::paste()
{
	assert( !curObjects_.empty() );

	// get data from clipboard
    QString data = clipboard_->text();
	// if nothing is in clipboard, do nothing
	if (data.isEmpty())
	{
		return false;
	}

	std::string str( data.toUtf8().constData(), data.size() );
	TextStream stream( str, std::ios::in );

	// deserialize values
	std::string tag;
	std::string hint;
	std::string valueTag;
	std::vector<Variant> values;
	while (!stream.eof())
	{
		stream.read( tag );
		if (tag == s_ValueHintTag)
		{
			stream.read(hint);
			stream.read(valueTag);
			assert( valueTag == s_ValueTag );
		}
		else
		{
			assert( tag == s_ValueTag);
		}
		Variant v;
		if (!deserializeData( stream, v ))
		{
			assert( false );
			return false;
		}
		values.emplace_back( std::move( v ) );
	}

	if (values.empty())
	{
		return false;
	}
	// paste value
	commandSystem_->beginBatchCommand();
	bool bSuccess = false;
	std::vector< ICopyableObject* >::iterator iter;
	for (iter = curObjects_.begin(); iter != curObjects_.end(); ++iter)
	{
		assert( *iter != nullptr );
		Variant value = (*iter)->getData();
		
		if (value.typeIs<Collection>())
		{
			Collection collection;
			bool isOk = value.tryCast( collection );
			if (!isOk)
			{
				break;
			}
			size_t i = 0;
			for (auto & v : values)
			{
				assert( i < collection.size() );
				collection[i++] = v;
			}
			value = collection;
		}
		else
		{
			value = values[0];
		}
		if ((*iter)->setData( value ))
		{
			bSuccess = true;
		}
	}
	if (bSuccess)
	{
		commandSystem_->endBatchCommand();
	}
	else
	{
		commandSystem_->abortBatchCommand();
	}

	return bSuccess;
}


//==============================================================================
bool QtCopyPasteManager::canCopy() const
{
	return clipboard_ && !curObjects_.empty();
}


//==============================================================================
bool QtCopyPasteManager::canPaste() const
{
	return clipboard_ && !curObjects_.empty();
}


//==============================================================================
void QtCopyPasteManager::init( ISerializationManager * serializationMgr, ICommandManager * commandSystem )
{
	assert( serializationMgr && commandSystem );
	curObjects_.clear();
	serializationMgr_ = serializationMgr;
	commandSystem_ = commandSystem;
}

//==============================================================================
void QtCopyPasteManager::fini()
{
	curObjects_.clear();
	serializationMgr_ = nullptr;
	commandSystem_ = nullptr;
}


//==============================================================================
bool QtCopyPasteManager::serializeData( IDataStream& stream, const Variant & value )
{
	
	if (value.typeIs<Collection>())
	{
		Collection collection;
		bool isOk = value.tryCast( collection );
		if (!isOk)
		{
			return false;
		}
		bool br = true;
		for (auto it = collection.begin(), end = collection.end();
			(it != end) && br; ++it )
		{
			auto v = it.value();
			br = serializeData( stream, v );
		}
		return br;
	}
	else
	{
		stream.write(s_ValueTag);
		stream.write( value.type()->name() );
	return serializationMgr_->serialize( stream, value );
	}
	
}


//==============================================================================
bool QtCopyPasteManager::deserializeData( IDataStream& stream, Variant& value )
{
	std::string valueType;
	stream.read( valueType );
	const MetaType* metaType = Variant::getMetaTypeManager()->findType( valueType.c_str() );
	Variant variant( metaType );
	bool br = serializationMgr_->deserialize( stream, variant );
	if (br)
	{
		value = variant;
	}
	return br;
}

