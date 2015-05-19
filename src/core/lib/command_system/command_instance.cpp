#include "command_instance.hpp"
#include "reflected_command.hpp"
#include "command_system_provider.hpp"

#include "reflection/generic/generic_object.hpp"
#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/i_definition_manager.hpp"
#include "reflection/i_object_manager.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/property_accessor_listener.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/property_iterator.hpp"
#include "reflection/interfaces/i_base_property.hpp"

#include "scoped_disable_command_recording.hpp"

#include "serialization/resizing_memory_stream.hpp"
#include "serialization/datastream_adapter.h"
#include "serialization/serializer/i_serialization_manager.hpp"


//TODO: Switch to multiplatform wait handles, possibly C++11
#if defined( _WIN32 )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif // defined( _WIN32 )



namespace
{
	

	struct ReflectionPropertyUndoRedoHelper
	{
		RefObjectId objectId_;		// object id
		std::string propertyPath_;	// property path
		std::string		propertyTypeName_;	// property type
		Variant		preValue_;		// pre value
		Variant     postValue_;		// post value
	};

	//==========================================================================
	class PropertyAccessorWrapper
		: public PropertyAccessorListener
	{
	public:
		PropertyAccessorWrapper(
			UndoRedoHelperList & undoRedoHelperList,
			const std::thread::id& commandThreadId )
			: undoRedoHelperList_( undoRedoHelperList ) 
			, commandThreadId_( commandThreadId )
		{
		}

		//======================================================================
		void preSetValue(
			const PropertyAccessor & accessor, const Variant & value ) override
		{
			if (ScopedDisableCommandRecording::isIgnored())
			{
				return;
			}
			assert( (std::this_thread::get_id() == commandThreadId_) &&
				"To record undo/redo data, properties must be changed in a"
				"command on the command thread" );

			const auto & obj = accessor.getRootObject();
			assert( obj != nullptr );
			RefObjectId id;
			bool ok = obj.getId( id );
			assert( ok );
			const char * propertyPath = accessor.getFullPath();
			const TypeId type = accessor.getType();
			Variant prevalue = accessor.getValue();
			auto pHelper = this->findUndoRedoHelper( id, propertyPath, type );
			if (pHelper != nullptr)
			{
				return;
			}
			ReflectionPropertyUndoRedoHelper helper;
			helper.objectId_ = id;
			helper.propertyPath_ = propertyPath;
			helper.propertyTypeName_ = type.getName();
			helper.preValue_ = prevalue;
			undoRedoHelperList_.emplace_back( helper );
		}


		//======================================================================
		void postSetValue(
			const PropertyAccessor & accessor, const Variant & value ) override
		{
			if (ScopedDisableCommandRecording::isIgnored())
			{
				return;
			}
			assert( (std::this_thread::get_id() == commandThreadId_) &&
				"To record undo/redo data, properties must be changed in a"
				"command on the command thread" );

			const auto & obj = accessor.getRootObject();
			assert( obj != nullptr );
			RefObjectId id;
			bool ok = obj.getId( id );
			assert( ok );
			const char * propertyPath = accessor.getFullPath();
			const TypeId type = accessor.getType();
			auto pHelper = this->findUndoRedoHelper( id, propertyPath, type );
			assert( pHelper != nullptr );
			pHelper->postValue_ = value;
		}

	private:
		ReflectionPropertyUndoRedoHelper* findUndoRedoHelper( 
			const RefObjectId & id, const char * propertyPath, const TypeId & type )
		{
			ReflectionPropertyUndoRedoHelper* helper = nullptr;
			for (auto& findIt : undoRedoHelperList_)
			{
				if ((findIt.objectId_ == id) && 
					(findIt.propertyPath_ == propertyPath) && 
					(findIt.propertyTypeName_ == type.getName()))
				{
					helper = &findIt;
					break;
				}
			}
			return helper;
		}
	private:
		UndoRedoHelperList &	undoRedoHelperList_;
		const std::thread::id& commandThreadId_;
	};


	/**
	 *	Reads reflected properties from a data stream into a cache.
	 *	@param outPropertyCache cache to fill.
	 *	@param stream data stream to read from.
	 *	@return success.
	 */
	bool loadReflectedProperties( UndoRedoHelperList & outPropertyCache,
		IDataStream & stream,
		IObjectManager & objectManager )
	{
		if (stream.eof())
		{
			return true;
		}
		auto pSerializationMgr = objectManager.getSerializationManager();
		assert( pSerializationMgr != nullptr );
		const char * propertyHeaderTag = CommandInstance::getPropertyHeaderTag();
		while (!stream.eof())
		{
			outPropertyCache.emplace_back();
			auto& helper = outPropertyCache.back();

			// read header
			std::string header;
			stream.read( header );
			assert( header == propertyHeaderTag );
			// read root object id
			std::string id;
			stream.read( id );
			assert( !id.empty() );
			helper.objectId_ = RefObjectId( id );

			// read property fullpath
			stream.read( helper.propertyPath_ );
			const auto& fullPath = helper.propertyPath_;

			// read property type
			stream.read( helper.propertyTypeName_ );
			const TypeId type( helper.propertyTypeName_.c_str() );

			ObjectHandle object = objectManager.getObject( helper.objectId_ );
			assert( object.isValid() );
			if (!object.isValid())
			{
				helper.preValue_ = Variant( "Unknown" );
				return true;
			}

			PropertyAccessor pa = object.getDefinition()->bindProperty(
				fullPath.c_str(), object );
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
				return false;
			}

			//read id
			std::string subId;
			stream.read( subId );
			Variant value = pa.getValue();
			if (ReflectionUtilities::isStruct(pa))
			{
				assert( metaType == value.type() );
				pSerializationMgr->deserialize( stream, value );
				helper.preValue_ = value;
			}
			else
			{
				if (!subId.empty())
				{
					auto obj = objectManager.getObject( subId );
					if (obj == nullptr)
					{
						assert( false );
						return false;
					}
					else
					{
						helper.preValue_ = obj;
					}
				}
				else
				{
					Variant variant( metaType );
					if (!variant.isVoid())
					{
						pSerializationMgr->deserialize( stream, variant );
						helper.preValue_ = variant;
					}
				}
			}
		}
		
		return true;
	}

	/**
	 *	Bind the property for context object
	 *	@param handle: a object which has definition.
	 *	@param propertyPath: the property path which object try to bind to.
	 *	@param o_Pa: PropertyAccessor to fill.
	 */
	void resolveProperty( const ObjectHandle & handle, const IClassDefinition & classDef, const char * propertyPath, PropertyAccessor & o_Pa )
	{
		o_Pa = handle.getDefinition()->bindProperty( propertyPath, handle );
		if(o_Pa.isValid())
		{
			return;
		}
		const PropertyIteratorRange& props = classDef.allProperties();
		std::vector< PropertyAccessor > pas;
		for (PropertyIterator pi = props.begin(), end = props.end(); 
			pi != end; ++pi)
		{
			std::string parentPath = pi->getName();
			const PropertyAccessor& prop = classDef.bindProperty( 
				parentPath.c_str(), handle );
			assert( prop.isValid() );
			const Variant & value = prop.getValue();
			if (value.typeIs<ObjectHandle>())
			{
				ObjectHandle subHandle;
				bool isOk = value.tryCast( subHandle );
				assert( isOk );
				if ( (subHandle == nullptr) || (subHandle.getDefinition() == nullptr) )
				{
					continue;
				}
				parentPath = parentPath + "." + propertyPath;
				resolveProperty( handle, *subHandle.getDefinition(), parentPath.c_str(), o_Pa );
				if(o_Pa.isValid())
				{
					return;
				}
			}
		}
	}


	/**
	 *	Bind the property for context object
	 *  bind strategy:
	 *  suppose the property path stored in macro is a.b.c.d.e
	 *  if context object has a property which path is one of 
	 *  the following value, we could bind it
	 *  $X.a.b.c.d.e or $X.b.c.d.e or $X.c.d.e or $X.d.e or$X.e
	 *  $X could be any of property path including empty string
	 *	@param contextObject: a context object which try to bind property with.
	 *	@param propertyPath: the property path which context object try to bind to.
	 *	@return PropertyAccessor object.
	 */
	PropertyAccessor bindContextObjectProperty( const ObjectHandle & contextObject, const char * propertyPath )
	{
		assert( contextObject != nullptr );
		const auto classDef = contextObject.getDefinition();
		assert( classDef != nullptr );
		std::string tmp = propertyPath;
		std::vector<std::string> paths;
		paths.push_back( tmp );

		char * pch;
		pch = strtok (const_cast<char *>(tmp.c_str()),".");
		if (pch != NULL)
		{
			pch = strtok (NULL, ".");
			while (pch != NULL)
			{
				paths.push_back( pch );
				pch = strtok (NULL, ".");
			}
		}
		PropertyAccessor o_Pa;
		for (auto & path : paths)
		{
			resolveProperty( contextObject, *classDef, path.c_str(), o_Pa );
			if (o_Pa.isValid())
			{
				break;
			}
		}
		return std::move( o_Pa );
	}

	/**
	 *	Applies properties stored in a cache onto the reflection system.
	 *	@param propertyCache cache to use.
	 *	@param stream data stream to read from.
	 *	@return success.
	 */
	bool applyReflectedProperties( const UndoRedoHelperList & propertyCache,
		IObjectManager & objectManager,
		ObjectHandle & contextObject )
	{
		for (const auto& helper : propertyCache)
		{
			// read root object id
			const auto& id = helper.objectId_;
			
			// read property fullpath
			const auto& fullPath = helper.propertyPath_;
			// read property type
			const auto& typeName = helper.propertyTypeName_;
			const TypeId type( typeName.c_str() );

			PropertyAccessor pa;
			if ((contextObject != nullptr) && (contextObject.getDefinition() != nullptr))
			{
				pa = bindContextObjectProperty( contextObject, fullPath.c_str() );
			}
			else
			{
				ObjectHandle object;
				object = objectManager.getObject( id );
				assert( object != nullptr );
				pa = object.getDefinition()->bindProperty( fullPath.c_str(), object );
			}

			assert( pa.isValid() );
			TypeId propType = pa.getType();

			assert( type == propType );

			// read value type
			const auto& value = helper.preValue_;
			pa.setValue( value );
		}
		
		return true;
	}


	bool performReflectedUndoRedo( ResizingMemoryStream& data,
		const char* expectedFormatHeader,
		IObjectManager & objectManager,
		ObjectHandle & contextObject )
	{
		data.seek( 0 );
		std::string formatHeader;
		data.read( formatHeader );
		assert( formatHeader == expectedFormatHeader );

		UndoRedoHelperList propertyCache;
		const bool loaded = loadReflectedProperties( propertyCache,
			data,
			objectManager );
		const bool applied = applyReflectedProperties( propertyCache,
			objectManager,
			contextObject );
		return (loaded && applied);
	}
}


//==============================================================================
const char * CommandInstance::getUndoStreamHeaderTag()
{
	static const char * s_ReflectionUndoData = "ReflectionUndoData";
	return s_ReflectionUndoData;
}

//==============================================================================
const char * CommandInstance::getRedoStreamHeaderTag()
{
	static const char * s_ReflectionRedoData = "ReflectionRedoData";
	return s_ReflectionRedoData;
}

//==============================================================================
const char * CommandInstance::getPropertyHeaderTag()
{
	static const char * s_ReflectionPropertyData = "ReflectionPropertyData";
	return s_ReflectionPropertyData;
}

//==============================================================================
CommandInstance::CommandInstance()
	: status_( Complete )
	, commandEvent_( INVALID_HANDLE_VALUE )
	, arguments_( nullptr )
	, pCmdSysProvider_( nullptr )
	, commandId_("")
	, contextObject_( nullptr )
{
	
}


//==============================================================================
/*virtual */void CommandInstance::init( const std::thread::id& commandThreadId )
{
	paListener_ = std::make_shared< PropertyAccessorWrapper >(
		undoRedoHelperList_,
		commandThreadId );

	const char * undoStreamHeaderTag = CommandInstance::getUndoStreamHeaderTag();
	const char * redoStreamHeaderTag = CommandInstance::getRedoStreamHeaderTag();

	undoData_.seek( 0 );
	undoData_.write( undoStreamHeaderTag );

	redoData_.seek( 0 );
	redoData_.write( redoStreamHeaderTag );
}


//==============================================================================
CommandInstance::~CommandInstance()
{
	assert( undoRedoHelperList_.empty() );
	paListener_ = nullptr;
	if (commandEvent_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle( commandEvent_ );
	}
}


//==============================================================================
void CommandInstance::cancel()
{
}

//==============================================================================
ObjectHandle CommandInstance::waitForCompletion()
{
	if (status_ != Complete)
	{
		commandEvent_ = CreateEvent( 0, true, false, NULL );
		if (status_ == Complete)
		{
			return returnValue_;
		}
		while( ::WaitForSingleObject( commandEvent_, 250 ) )
		{
			getCommand()->fireProgressMade( *this );
		}
	}
	return returnValue_;
}


//==============================================================================
void CommandInstance::setArguments( const ObjectHandle & arguments )
{
	arguments_ = arguments ;
}


//==============================================================================
const char * CommandInstance::getCommandId() const
{
	return commandId_.c_str();
}


//==============================================================================
void CommandInstance::setCommandId( const char * commandName )
{
	commandId_ = commandName;
}

//==============================================================================
Command * CommandInstance::getCommand()
{
	assert( pCmdSysProvider_ != nullptr );
	Command * pCommand = pCmdSysProvider_->findCommand( commandId_.c_str() );
	assert( pCommand != nullptr );
	return pCommand;
}


const Command * CommandInstance::getCommand() const
{
	assert( pCmdSysProvider_ != nullptr );
	Command * pCommand = pCmdSysProvider_->findCommand( commandId_.c_str() );
	assert( pCommand != nullptr );
	return pCommand;
}

//==============================================================================
/*virtual */void CommandInstance::setStatus( ExecutionStatus status )
{
	status_ = status;
	getCommand()->fireCommandStatusChanged( *this );
	if(status_ == Complete)
	{
		if (commandEvent_ != INVALID_HANDLE_VALUE)
		{
			SetEvent( commandEvent_ );
		}
	}
}


/**
 *	Generates display data from the undo/redo data stream.
 *	@return a copy of display data for the UI.
 */
ObjectHandle CommandInstance::createDisplayData() const
{
	auto pDefinitionManager =
		this->getDefinition().getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	auto& definitionManager = (*pDefinitionManager);

	auto pObjectManager = definitionManager.getObjectManager();
	assert( pObjectManager != nullptr );
	auto& objectManager = (*pObjectManager);
	const char * undoStreamHeaderTag = CommandInstance::getUndoStreamHeaderTag();
	const char * redoStreamHeaderTag = CommandInstance::getRedoStreamHeaderTag();
	UndoRedoHelperList propertyCache;
	{
		// Need to read undo/redo data separately and then consolidate it into
		// propertyCache.
		// TODO NGT-281 change loadReflectedProperties to load into one cache.
		UndoRedoHelperList undoPropertyCache;
		{
			// Make a copy because this function should not modify stream contents
			// TODO ResizingMemoryStream const read implementation
			ResizingMemoryStream stream(
				static_cast< const char* >( undoData_.rawBuffer() ),
				undoData_.size() );
			assert( !stream.eof() );

			// Read property header
			std::string header;
			header.reserve( strlen( undoStreamHeaderTag ) );
			stream.read( header );
			assert( header == undoStreamHeaderTag );

			// Read properties into cache
			const bool reflectedPropertiesLoaded = loadReflectedProperties(
				undoPropertyCache,
				stream,
				objectManager );
			if (!reflectedPropertiesLoaded)
			{
				return ObjectHandle( nullptr );
			}
		}

		UndoRedoHelperList redoPropertyCache;
		{
			// Make a copy because this function should not modify stream contents
			// TODO ResizingMemoryStream const read implementation
			ResizingMemoryStream stream(
				static_cast< const char* >( redoData_.rawBuffer() ),
				redoData_.size() );
			assert( !stream.eof() );

			// Read property header
			std::string header;
			header.reserve( strlen( redoStreamHeaderTag ) );
			stream.read( header );
			assert( header == redoStreamHeaderTag );

			// Read properties into cache
			const bool reflectedPropertiesLoaded = loadReflectedProperties(
				redoPropertyCache,
				stream,
				objectManager );
			if (!reflectedPropertiesLoaded)
			{
				return ObjectHandle( nullptr );
			}
		}

		// Merge the undo/redo caches into one
		for (const auto& redoHelper : redoPropertyCache)
		{
			propertyCache.emplace_back();
			auto& helper = propertyCache.back();

			helper.objectId_ = redoHelper.objectId_;
			helper.propertyPath_ = redoHelper.propertyPath_;
			helper.propertyTypeName_ = redoHelper.propertyTypeName_;
			helper.postValue_ = redoHelper.preValue_;
		}
		size_t index = 0;
		for (const auto& undoHelper : undoPropertyCache)
		{
			auto& helper = propertyCache[ index ];
			++index;

			assert( helper.objectId_ == undoHelper.objectId_ );
			assert( helper.propertyPath_ == undoHelper.propertyPath_ );
			assert( helper.propertyTypeName_ == undoHelper.propertyTypeName_ );
			helper.preValue_ = undoHelper.preValue_;
		}
	}

	// Custom command override
	const bool commandPropertiesLoaded = this->getCommand()->createDisplayData(
		propertyCache,
		undoData_,
		redoData_ );
	if (!commandPropertiesLoaded)
	{
		return ObjectHandle( nullptr );
	}

	// Create display object from cache
	assert( !propertyCache.empty() );

	{
		// Do not record undo/redo data for the object which displays undo/redo
		// data
		ScopedDisableCommandRecording ignore;

		auto handle = GenericObject::create( definitionManager );
		assert( handle.get() != nullptr );
		auto& genericObject = (*handle);

		// Single command
		if (propertyCache.size() == 1)
		{
			auto& helper = propertyCache.at( 0 );

			genericObject.set( "Id", helper.objectId_ );
			genericObject.set( "Name", helper.propertyPath_ );
			genericObject.set( "Type", helper.propertyTypeName_ );
			genericObject.set( "PreValue", helper.preValue_ );
			genericObject.set( "PostValue", helper.postValue_ );
		}
		// NGT-349 TODO batch commands
		else
		{
			genericObject.set( "Name", "Batch" );
			genericObject.set( "Type", "Batch" );
		}

		return handle;
	}
}

//==============================================================================
void CommandInstance::undo()
{
	const auto pObjectManager = 
		this->getDefinition().getDefinitionManager()->getObjectManager();
	assert( pObjectManager != nullptr );
	const char * undoStreamHeaderTag = CommandInstance::getUndoStreamHeaderTag();
	const bool result = performReflectedUndoRedo( undoData_,
		undoStreamHeaderTag,
		(*pObjectManager),
		contextObject_ );
	if (result)
	{
		getCommand()->undo( undoData_ );
	}
}


//==============================================================================
void CommandInstance::redo()
{
	const auto pObjectManager = 
		this->getDefinition().getDefinitionManager()->getObjectManager();
	assert( pObjectManager != nullptr );
	const char * redoStreamHeaderTag = CommandInstance::getRedoStreamHeaderTag();
	const bool result = performReflectedUndoRedo( redoData_,
		redoStreamHeaderTag,
		(*pObjectManager),
		contextObject_ );
	if (result)
	{
		getCommand()->redo( redoData_ );
	}
	
}


//==============================================================================
void CommandInstance::execute()
{
	assert( status_ != Complete );
	setStatus( Running );
	returnValue_ = getCommand()->execute( arguments_ );
	setStatus( Complete );
	arguments_ = nullptr;
}

//==============================================================================
void CommandInstance::connectEvent()
{
	assert( paListener_ );
	getDefinition().getDefinitionManager()->registerPropertyAccessorListener( paListener_ );
}

//==============================================================================
void CommandInstance::disconnectEvent()
{
	assert( paListener_ );
	getDefinition().getDefinitionManager()->deregisterPropertyAccessorListener( paListener_ );
	for (const auto& helper : undoRedoHelperList_)
	{
		this->saveUndoRedoData( undoData_, helper );
		this->saveUndoRedoData( redoData_, helper, false );
	}
	undoRedoHelperList_.clear();
}


void CommandInstance::saveUndoRedoData( IDataStream & stream, const ReflectionPropertyUndoRedoHelper& helper, bool undoData )
{
	IObjectManager* objManager = 
		getDefinition().getDefinitionManager()->getObjectManager();
	ISerializationManager * serializationMgr = objManager->getSerializationManager();
	const char * propertyHeaderTag = CommandInstance::getPropertyHeaderTag();
	//write header
	stream.write( propertyHeaderTag );
	// write root object id
	stream.write( helper.objectId_.toString() );
	// write property fullPath
	stream.write( helper.propertyPath_ );
	// write property type
	stream.write( helper.propertyTypeName_ );
	// write value type
	Variant value;
	if (undoData)
	{
		value = helper.preValue_;
	}
	else
	{
		value = helper.postValue_;
	}
	stream.write( value.type()->name() );
	// write value

	bool hasId = value.typeIs< ObjectHandle >();
	if (hasId)
	{
		ObjectHandle provider;
		bool isOk = value.tryCast( provider );
		assert( isOk );
		if (provider.isValid())
		{
			RefObjectId id;
			hasId = provider.getId( id );
			if (hasId)
			{
				stream.write( id.toString() );
			}
			else
			{	
				hasId = false;
			}
		}
		else
		{
			hasId = false;
		}
	}
	if (!hasId)
	{
		// not a managed object
		stream.write( "" );
		serializationMgr->serialize( stream, value );
	}
}

void CommandInstance::getUndoData(std::string * undoData) const
{
	undoData->assign( static_cast<const char *>(undoData_.rawBuffer()), undoData_.size());
}
void CommandInstance::setUndoData( const std::string & undoData )
{
	undoData_.resetData();
	size_t size = undoData_.writeRaw( undoData.data(), undoData.length() );
	assert(size == undoData.length());
}
void CommandInstance::getRedoData(std::string * redoData) const
{
	redoData->assign( static_cast<const char *>(redoData_.rawBuffer()), redoData_.size());
}
void CommandInstance::setRedoData( const std::string & redoData )
{
	redoData_.resetData();
	size_t size = redoData_.writeRaw( redoData.data(), redoData.length() );
	assert(size == redoData.length());
}

void CommandInstance::setContextObject( const ObjectHandle & contextObject )
{
	contextObject_ = contextObject;
}

void CommandInstance::setCommandSystemProvider( CommandSystemProvider * pCmdSysProvider )
{
	pCmdSysProvider_ = pCmdSysProvider;
}
