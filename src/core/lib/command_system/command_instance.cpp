#include "command_instance.hpp"
#include "command.hpp"
#include "i_command_manager.hpp"

#include "data_model/collection_model.hpp"

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

#include "logging/logging.hpp"


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
			helper.preValue_ = std::move( prevalue );
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
			 Variant postValue = accessor.getValue();
			auto pHelper = this->findUndoRedoHelper( id, propertyPath, type );
			assert( pHelper != nullptr );
			pHelper->postValue_ = std::move( postValue );
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
	 *	Function object which holds the algorithm for loading new properties
	 *	into a property cache.
	 */
	class PropertyCacheFiller
	{
	public:
		virtual ReflectionPropertyUndoRedoHelper& getNext() = 0;
	};


	/**
	 *	Function object which will add a new element to the cache and return it.
	 */
	class PropertyCacheCreator
		: public PropertyCacheFiller
	{
	public:
		PropertyCacheCreator( UndoRedoHelperList & propertyCache )
			: propertyCache_( propertyCache )
		{
		}
		ReflectionPropertyUndoRedoHelper& getNext() override
		{
			propertyCache_.emplace_back();
			return propertyCache_.back();
		}
	private:
		UndoRedoHelperList& propertyCache_;
	};


	/**
	 *	Function object which will iterate over elements in a cache so they
	 *	can be overwritten.
	 */
	class PropertyCacheIterator
		: public PropertyCacheFiller
	{
	public:
		PropertyCacheIterator( UndoRedoHelperList & propertyCache )
			: propertyCache_( propertyCache )
			, itr_( propertyCache.begin() )
		{
		}

		ReflectionPropertyUndoRedoHelper& getNext() override
		{
			assert( itr_ != propertyCache_.end() );
			auto& helper = (*itr_);
			++itr_;
			return helper;
		}
	private:
		UndoRedoHelperList& propertyCache_;
		UndoRedoHelperList::iterator itr_;
	};


	/**
	 *	Function pointer for getting either pre- or post-values from the
	 *	property cache.
	 */
	typedef Variant (*PropertyGetter)( const ReflectionPropertyUndoRedoHelper& );


	/**
	 *	Function pointer for setting either pre- or post-values on the
	 *	property cache.
	 */
	typedef void (*PropertySetter)( ReflectionPropertyUndoRedoHelper&,
		const Variant& );


	/**
	 *	Function for getting undo data from the property cache.
	 *	@param helper the cache which contains the undo value.
	 *	@return the value from the cache.
	 */
	Variant undoPropertyGetter( const ReflectionPropertyUndoRedoHelper& helper )
	{
		return helper.preValue_;
	}


	/**
	 *	Function for getting redo data from the property cache.
	 *	@param helper the cache which contains the redo value.
	 *	@return the value from the cache.
	 */
	Variant redoPropertyGetter( const ReflectionPropertyUndoRedoHelper& helper )
	{
		return helper.postValue_;
	}


	/**
	 *	Function for setting undo data on the property cache.
	 *	@param helper the cache on which to set the value.
	 *	@param value the value to set on the cache.
	 */
	void undoPropertySetter( ReflectionPropertyUndoRedoHelper& helper,
		const Variant& value )
	{
		helper.preValue_ = value;
	}


	/**
	 *	Function for setting redo data on the property cache.
	 *	@param helper the cache on which to set the value.
	 *	@param value the value to set on the cache.
	 */
	void redoPropertySetter( ReflectionPropertyUndoRedoHelper& helper,
		const Variant& value )
	{
		helper.postValue_ = value;
	}


	/**
	 *	Reads reflected properties from a data stream into a cache.
	 *	@param outPropertyCache cache to fill.
	 *	@param stream data stream to read from.
	 *	@param propertySetter function used to set undo or redo data on the
	 *		cache.
	 *	@return success.
	 */
	bool loadReflectedProperties( PropertyCacheFiller & outPropertyCache,
		IDataStream & stream,
		PropertySetter propertySetter,
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
			auto& helper = outPropertyCache.getNext();

			// read header
			std::string header;
			stream.read( header );
			if (header != propertyHeaderTag)
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid header\n");
				return true;
			}

			// read root object id
			std::string id;
			stream.read( id );
			if (id.empty())
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid ID\n");
				return true;
			}
			helper.objectId_ = RefObjectId( id );

			// read property fullpath
			stream.read( helper.propertyPath_ );
			const auto& fullPath = helper.propertyPath_;

			// read property type
			stream.read( helper.propertyTypeName_ );
			const TypeId type( helper.propertyTypeName_.c_str() );

			ObjectHandle object = objectManager.getObject( helper.objectId_ );
			if (!object.isValid())
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid object\n");
				return true;
			}

			PropertyAccessor pa = object.getDefinition()->bindProperty(
				fullPath.c_str(), object );
			if ( !pa.isValid() )
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid property\n");
				return true;
			}

			TypeId propType = pa.getType();

			if ( type != propType )
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid property type\n");
				return true;
			}

			// read value type
			std::string valueType;
			stream.read( valueType );
			// read value
			const MetaType * metaType = Variant::getMetaTypeManager()->findType(
				valueType.c_str() );
			if (metaType == nullptr)
			{
				propertySetter( helper, Variant( "Unknown" ) );
				NGT_TRACE_MSG("Failed to load reflected properties - invalid meta type\n");
				return true;
			}

			Variant value = pa.getValue();
			if (ReflectionUtilities::isStruct(pa))
			{
				if ( metaType != value.type() )
				{
					propertySetter( helper, Variant( "Unknown" ) );
					NGT_TRACE_MSG("Failed to load reflected properties - invalid value type\n");
					return true;
				}
				pSerializationMgr->deserialize( stream, value );
				propertySetter( helper, value );
			}
			else
			{
				Variant variant( metaType );
				if (!variant.isVoid())
				{
					pSerializationMgr->deserialize( stream, variant );
					propertySetter( helper, variant );
				}
			}
		}
		
		return true;
	}


	/**
	 *	Reads reflected properties from a data stream into a cache.
	 *	@param outPropertyCache cache to fill.
	 *	@param undoStream data stream from which to read.
	 *	@param redoStream data stream from which to read.
	 *	@return success.
	 */
	bool loadReflectedProperties( UndoRedoHelperList & outPropertyCache,
		IDataStream & undoStream,
		IDataStream & redoStream,
		IObjectManager & objectManager )
	{
		const bool undoSuccess = loadReflectedProperties(
			PropertyCacheCreator( outPropertyCache ),
			undoStream,
			&undoPropertySetter,
			objectManager );

		const bool redoSuccess = loadReflectedProperties(
			PropertyCacheIterator( outPropertyCache ),
			redoStream,
			&redoPropertySetter,
			objectManager );

		return (undoSuccess && redoSuccess);
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
		PropertyGetter propertyGetter,
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
				ObjectHandle object = objectManager.getObject( id );
				if ( object == nullptr )
				{
					NGT_TRACE_MSG( "Failed to apply reflected property - object is null\n" );
					return false;
				}
				pa = object.getDefinition()->bindProperty( fullPath.c_str(), object );
			}

			assert( pa.isValid() );
			TypeId propType = pa.getType();

			assert( type == propType );

			// read value type
			const auto& value = propertyGetter( helper );
			bool br = pa.setValue( value );
			if (!br)
			{
				return false;
			}
		}
		
		return true;
	}


	bool performReflectedUndoRedo( ResizingMemoryStream& data,
		PropertyGetter propertyGetter,
		PropertySetter propertySetter,
		const char* expectedFormatHeader,
		IObjectManager & objectManager,
		ObjectHandle & contextObject )
	{
		data.seek( 0 );
		std::string formatHeader;
		data.read( formatHeader );
		assert( formatHeader == expectedFormatHeader );

		UndoRedoHelperList propertyCache;
		const bool loaded = loadReflectedProperties(
			PropertyCacheCreator( propertyCache ),
			data,
			propertySetter,
			objectManager );
		const bool applied = applyReflectedProperties( propertyCache,
			propertyGetter,
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
	, arguments_( nullptr )
	, pCmdSysProvider_( nullptr )
	, commandId_("")
	, contextObject_( nullptr )
	, errorCode_( CommandErrorCode::NO_ERROR )
{
}

//==============================================================================
CommandInstance::CommandInstance( const CommandInstance& )
{
	assert(!"Not copyable");
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
}


//==============================================================================
void CommandInstance::cancel()
{
}


//==============================================================================
void CommandInstance::waitForCompletion()
{
	std::unique_lock<std::mutex> lock( mutex_ );

	while( !completeStatus_.wait_for(
		lock,
		std::chrono::milliseconds( 1 ),
		[this] { return status_ == Complete; } ) )
	{
		getCommand()->fireProgressMade( *this );
	}
}


//==============================================================================
CommandErrorCode CommandInstance::getErrorCode() const
{
	if (children_.empty())
	{
		return errorCode_;
	}

	//batchcommand(parent command) failure only when:
	//all sub-commands failed(errorcode != NO_ERROR) or as long as one of sub-commands' errorcode == ABORTED
	CommandErrorCode errorCode = CommandErrorCode::ABORTED;
	for (const auto & child : children_)
	{
		CommandErrorCode childErrorCode = child->getErrorCode();
		if (childErrorCode == CommandErrorCode::NO_ERROR)
		{
			errorCode = childErrorCode;
			continue;
		}
		if (childErrorCode == CommandErrorCode::ABORTED)
		{
			errorCode = childErrorCode;
			break;
		}
	}
	return errorCode;
}

//==============================================================================
bool CommandInstance::isMultiCommand() const
{
	return !children_.empty();
}

//==============================================================================
void CommandInstance::setArguments( const ObjectHandle & arguments )
{
	arguments_ = arguments;
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
	// assume mutex_ is held by current thread

	status_ = status;
	getCommand()->fireCommandStatusChanged( *this );
	if (status_ == Complete)
	{
		completeStatus_.notify_all();
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

		// Make a copy because this function should not modify stream contents
		// TODO ResizingMemoryStream const read implementation
		ResizingMemoryStream undoStream(
			static_cast< const char* >( undoData_.rawBuffer() ),
			undoData_.size() );
		assert( !undoStream.eof() );

		// Read property header
		std::string undoHeader;
		undoHeader.reserve( strlen( undoStreamHeaderTag ) );
		undoStream.read( undoHeader );
		assert( undoHeader == undoStreamHeaderTag );

		// Make a copy because this function should not modify stream contents
		// TODO ResizingMemoryStream const read implementation
		ResizingMemoryStream redoStream(
			static_cast< const char* >( redoData_.rawBuffer() ),
			redoData_.size() );
		assert( !redoStream.eof() );

		// Read property header
		std::string redoHeader;
		redoHeader.reserve( strlen( redoStreamHeaderTag ) );
		redoStream.read( redoHeader );
		assert( redoHeader == redoStreamHeaderTag );

		// Read properties into cache
		const bool reflectedPropertiesLoaded = loadReflectedProperties(
			propertyCache,
			undoStream,
			redoStream,
			objectManager );
		if (!reflectedPropertiesLoaded)
		{
			return ObjectHandle( nullptr );
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
	{
		// Do not record undo/redo data for the object which displays undo/redo
		// data
		ScopedDisableCommandRecording ignore;

		auto handle = GenericObject::create( definitionManager );
		assert( handle.get() != nullptr );
		auto& genericObject = (*handle);

		// command with no property change
		if (propertyCache.empty())
		{
			genericObject.set( "Name", "Unknown" );
			genericObject.set( "Type", "Unknown" );
		}
		// Single command
		else if (propertyCache.size() == 1)
		{
			auto& helper = propertyCache.at( 0 );

			genericObject.set( "Id", helper.objectId_ );
			genericObject.set( "Name", helper.propertyPath_ );
			genericObject.set( "Type", helper.propertyTypeName_ );
			genericObject.set( "PreValue", helper.preValue_ );
			genericObject.set( "PostValue", helper.postValue_ );
		}
		else
		{
			genericObject.set( "Name", "Batch" );
			genericObject.set( "Type", "Batch" );

			// Need to create a CollectionHolder, otherwise
			// genericObject.set( "Children", children );
			// is unsafe, because it takes a reference
			// which will be deleted when children goes out of scope
			typedef std::vector< GenericObjectPtr > Children;
			auto collectionHolder =
				std::make_shared< CollectionHolder< Children > >();

			Children& children = collectionHolder->storage();
			children.reserve( propertyCache.size() );

			for (const auto& helper : propertyCache)
			{
				auto childHandle = GenericObject::create( definitionManager );
				assert( childHandle.get() != nullptr );

				auto& childObject = (*childHandle);
				childObject.set( "Id", helper.objectId_ );
				childObject.set( "Name", helper.propertyPath_ );
				childObject.set( "Type", helper.propertyTypeName_ );
				childObject.set( "PreValue", helper.preValue_ );
				childObject.set( "PostValue", helper.postValue_ );

				children.push_back( childHandle );
			}

			// Convert CollectionHolder to Collection
			Collection childrenCollection( collectionHolder );
			genericObject.set( "Children", childrenCollection );
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
		&undoPropertyGetter,
		&undoPropertySetter,
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
		&redoPropertyGetter,
		&redoPropertySetter,
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
	returnValue_ = getCommand()->execute( arguments_ );
	auto errorCode = returnValue_.getBase<CommandErrorCode>();
	if (errorCode != nullptr)
	{
		errorCode_ = *errorCode;
	}
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
	serializationMgr->serialize( stream, value );

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

void CommandInstance::setCommandSystemProvider( ICommandManager * pCmdSysProvider )
{
	pCmdSysProvider_ = pCmdSysProvider;
}
