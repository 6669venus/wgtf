#include "macro_object.hpp"
#include "command_system/command_system_provider.hpp"
#include <cassert>
#include "reflection/i_definition_manager.hpp"
#include "reflection/i_object_manager.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "compound_command.hpp"
#include "logging/logging.hpp"
#include "data_model/generic_list.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/property_accessor.hpp"
#include "string_utils/string_utils.hpp"
#include "serialization/resizing_memory_stream.hpp"
#include "variant/meta_type.hpp"
#include <sstream>
#include <codecvt>

//==============================================================================
class ContextObjectListItem : public GenericListItem
{
public:
	ContextObjectListItem( const Variant& value )
		: GenericListItem( value )
		, displayName_( "Unknown" )
	{

	}
	ContextObjectListItem( Variant&& value )
		: GenericListItem( std::forward<Variant&&>( value ) )
		, displayName_( "Unknown" )
	{

	}
	// IItem
	const char * getDisplayText( int column ) const override
	{
		const Variant & value = this->value<const Variant &>();
		if (value.typeIs<ObjectHandle>())
		{
			ObjectHandle objHandle;
			bool isOk = value.tryCast( objHandle );
			assert( isOk );
			auto metaName =
				findFirstMetaData< MetaDisplayNameObj >( *objHandle.getDefinition() );
			if (metaName != nullptr)
			{
				std::wstring_convert< Utf16to8Facet > conversion( 
					Utf16to8Facet::create() );

				displayName_ = conversion.to_bytes( metaName->getDisplayName() );
			}
			else
			{
				const auto classDef = objHandle.getDefinition();
				if (classDef != nullptr)
				{
					if (classDef->isGeneric())
					{
						displayName_ = "GenericObject";
					}
					else
					{
						displayName_ = classDef->getName();
					}
				}
			}
		}
		return displayName_.c_str();
	}
private:
	//TODO: http://jira.bigworldtech.com/browse/NGT-434
	mutable std::string displayName_;
};


//==============================================================================
MacroEditObject::MacroEditObject()
	: commandInstanceIndex_( -1 )
	, objectId_( "" )
	, propertyPath_( "" )
	, propertyTypeName_( "" )
	, valueType_( nullptr )
	, valueObjectId_( "" )
{

}


//==============================================================================
const int & MacroEditObject::commandInstanceIndex() const
{
	return commandInstanceIndex_;
}


//==============================================================================
const char * MacroEditObject::objectId() const
{
	return objectId_.c_str();
}


//==============================================================================
const char * MacroEditObject::propertyPath() const
{
	return propertyPath_.c_str();
}


//==============================================================================
const char * MacroEditObject::propertyType() const
{
	return propertyTypeName_.c_str();
}


//==============================================================================
const MetaType * MacroEditObject::valueType() const
{
	return valueType_;
}


//==============================================================================
const char * MacroEditObject::valueObjectId() const
{
	return valueObjectId_.c_str();
}


//==============================================================================
const Variant & MacroEditObject::value() const
{
	return value_;
}


//==============================================================================
void MacroEditObject::commandInstanceIndex( const int & index)
{
	commandInstanceIndex_ = index;
}


//==============================================================================
void MacroEditObject::objectId( const char * id )
{
	objectId_ = id;
}


//==============================================================================
void MacroEditObject::propertyPath( const char * propertyPath )
{
	propertyPath_ = propertyPath;
}


//==============================================================================
void MacroEditObject::propertyType( const char * propertyType )
{
	propertyTypeName_ = propertyType;
}


//==============================================================================
void  MacroEditObject::valueType( const MetaType * valueType )
{
	valueType_ = valueType;
	value_ = Variant( valueType );
}


//==============================================================================
void MacroEditObject::valueObjectId( const char * valueObjectId )
{
	valueObjectId_ = valueObjectId;
}


//==============================================================================
void MacroEditObject::value( const Variant & value )
{
	if (valueType_ == value.type())
	{
		value_ = value;
	}
	else
	{
		// when setting from macro edit ui window, value is
		// a string type which may be different from original
		// type, we need to convert it back
		assert( !value_.isVoid() );
		std::string str;
		bool isOk = value.tryCast( str );
		assert( isOk );
		std::stringstream s(str);
		s >> value_;
	}
	
}


//==============================================================================
MacroObject::MacroObject()
	: commandSystem_( nullptr )
	, pDefManager_( nullptr )
	, cmdId_( "" )
	, macroName_( "" )
	, contextList_( nullptr )
	, currentContextObj_( nullptr )
	, macroEditObjectList_( nullptr )
{
}


//==============================================================================
void MacroObject::init( ICommandManager& commandSystem, IDefinitionManager & defManager, const char * cmdId )
{
	commandSystem_ = &commandSystem;
	pDefManager_ = &defManager;
	cmdId_ = cmdId;
	macroName_ = cmdId_;
}


//==============================================================================
const ObjectHandle & MacroObject::getContextObjects() const
{
	if(contextList_ != nullptr)
	{
		return contextList_;
	}
	std::unique_ptr<GenericList> objList( new GenericList() );
	std::vector<const ObjectHandle> objs;
	pDefManager_->getObjectManager()->getObjects( objs );
	for (auto & obj : objs)
	{
		auto def = obj.getDefinition();
		if (def == nullptr)
		{
			continue;
		}
		std::unique_ptr<ContextObjectListItem> item( new ContextObjectListItem( obj ) );
		objList->push_back( item.release() );
	}
	contextList_ = std::move( objList );
	return contextList_;
}


//==============================================================================
const ObjectHandle & MacroObject::getContextObject() const
{
	return currentContextObj_;
}

//==============================================================================
void MacroObject::setContextObject( const ObjectHandle & obj )
{
	currentContextObj_ = obj;
}


//==============================================================================
ObjectHandle MacroObject::executeMacro() const
{
	if( currentContextObj_ == nullptr)
	{
		NGT_ERROR_MSG( "Please select a context object. \n" );
		return nullptr;
	}
	assert( commandSystem_ != nullptr );
	ObjectHandleT<CompoundCommandArgument> arguments 
		= commandSystem_->createCommandArgumentT<CompoundCommandArgument>();
	arguments->setContextObject( currentContextObj_ );
	CommandInstancePtr ins = commandSystem_->queueCommand( cmdId_.c_str(), arguments );
	return ins;
}


//==============================================================================
ObjectHandle MacroObject::createEditData() const
{
	if (macroEditObjectList_ != nullptr)
	{
		return macroEditObjectList_;
	}

	assert( commandSystem_ != nullptr );
	assert( pDefManager_ != nullptr );
	auto pObjectManager = pDefManager_->getObjectManager();
	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	std::unique_ptr<GenericList> objList( new GenericList() );
	std::vector<CommandInstancePtr> instances = macro->getSubCommands();
	assert( !instances.empty());
	int commandInstanceIndex = 0;
	const char * redoStreamHeaderTag = CommandInstance::getRedoStreamHeaderTag();
	const char * propertyHeaderTag = CommandInstance::getPropertyHeaderTag();
	for( auto & ins : instances )
	{
		// Make a copy because this function should not modify stream contents
		// TODO ResizingMemoryStream const read implementation
		ResizingMemoryStream stream(
			static_cast< const char* >( ins->getRedoStream().rawBuffer() ),
			ins->getRedoStream().size() );
		assert( !stream.eof() );

		// Read property header
		std::string header;
		header.reserve( strlen( redoStreamHeaderTag ) );
		stream.read( header );
		assert( header == redoStreamHeaderTag );

		auto pSerializationMgr = pObjectManager->getSerializationManager();
		assert( pSerializationMgr != nullptr );
		while (!stream.eof())
		{
			// read header
			std::string sectionHeader;
			stream.read( sectionHeader );
			assert( sectionHeader == propertyHeaderTag );
			auto editObject = pDefManager_->createT<MacroEditObject>( false );
			// read root object id
			std::string id;
			stream.read( id );
			assert( !id.empty() );

			// read property fullpath
			std::string propertyPath;
			stream.read(propertyPath );

			// read property type
			std::string propertyTypeName;
			stream.read( propertyTypeName );
			const TypeId type( propertyTypeName.c_str() );

			ObjectHandle object = pObjectManager->getObject( RefObjectId( id ) );
			assert( object.isValid() );

			PropertyAccessor pa = object.getDefinition()->bindProperty(
				propertyPath.c_str(), object );
			assert( pa.isValid() );
			TypeId propType = pa.getType();

			assert( type == propType );

			// read value type
			std::string valueType;
			stream.read( valueType );
			// read value
			const MetaType * metaType = Variant::getMetaTypeManager()->findType(
				valueType.c_str() );
			if (metaType == nullptr)
			{
				assert( false );
			}

			//read id
			std::string subId;
			stream.read( subId );
			Variant value = pa.getValue();
			if (ReflectionUtilities::isStruct(pa))
			{
				assert( metaType == value.type() );
				pSerializationMgr->deserialize( stream, value );
			}
			else
			{
				if (!subId.empty())
				{
					auto obj = pObjectManager->getObject( subId );
					if (obj == nullptr)
					{
						assert( false );
					}
					else
					{
						value = obj;
					}
				}
				else
				{
					Variant variant( metaType );
					if (!variant.isVoid())
					{
						pSerializationMgr->deserialize( stream, variant );
						value = variant;
					}
				}
			}
			editObject->commandInstanceIndex( commandInstanceIndex );
			editObject->objectId( id.c_str() );
			editObject->propertyPath( propertyPath.c_str() );
			editObject->propertyType( propertyTypeName.c_str() );
			editObject->valueType( metaType );
			editObject->valueObjectId( subId.c_str() );
			editObject->value( value );
			objList->push_back( editObject );
		}
		commandInstanceIndex++;
	}
	macroEditObjectList_ = std::move( objList );
	return macroEditObjectList_;
}

//==============================================================================
ObjectHandle MacroObject::updateMacro() const
{
	assert( commandSystem_ != nullptr );
	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	auto serializationMgr = 
		pDefManager_->getObjectManager()->getSerializationManager();
	assert( serializationMgr != nullptr );
	std::vector<CommandInstancePtr> instances = macro->getSubCommands();
	std::vector<ResizingMemoryStream> streams;
	size_t count = instances.size();
	const char * redoStreamHeaderTag = CommandInstance::getRedoStreamHeaderTag();
	const char * propertyHeaderTag = CommandInstance::getPropertyHeaderTag();
	// prepare data stream
	for (size_t i = 0; i < count; i++)
	{
		ResizingMemoryStream stream;
		stream.write(redoStreamHeaderTag);
		streams.push_back( std::move( stream ) );
	}

	// write data to the stream
	GenericList* objList = macroEditObjectList_.getBase<GenericList>();
	for(GenericList::Iterator iter = objList->begin(); iter != objList->end(); ++iter)
	{
		const Variant & variant = (*iter).value<const Variant &>();
		ObjectHandleT<MacroEditObject> obj;
		bool isOk = variant.tryCast( obj );
		assert( isOk );
		size_t index = obj->commandInstanceIndex();
		assert( index < streams.size() );
		ResizingMemoryStream& stream = streams[index];
		stream.write( propertyHeaderTag );
		stream.write( obj->objectId() );
		stream.write( obj->propertyPath() );
		stream.write( obj->propertyType() );
		stream.write( obj->valueType()->name() );
		stream.write( obj->valueObjectId() );
		serializationMgr->serialize( stream, obj->value() );
	}

	// write back to macro sub commandInstance
	for (size_t i = 0; i < count; i++)
	{
		ResizingMemoryStream& stream = streams[i];
		CommandInstancePtr& ins = instances[i];
		ResizingMemoryStream & redoStream = 
			static_cast<ResizingMemoryStream&>(const_cast<IDataStream&>(ins->getRedoStream()));
		redoStream.resetData();
		redoStream.writeRaw( stream.rawBuffer(), stream.size() );
	}
	return nullptr;
}
