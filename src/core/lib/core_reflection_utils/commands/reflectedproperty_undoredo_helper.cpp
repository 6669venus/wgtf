#include "reflectedproperty_undoredo_helper.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_logging/logging.hpp"
#include <thread>

namespace RPURU = ReflectedPropertyUndoRedoUtility;
namespace
{

/**
*	Function object which holds the algorithm for loading new properties
*	into a property cache.
*/
class PropertyCacheFiller
{
public:
	virtual std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() = 0;
};


/**
*	Function object which will add a new element to the cache and return it.
*/
class PropertyCacheCreator
	: public PropertyCacheFiller
{
public:
	PropertyCacheCreator( RPURU::UndoRedoHelperList & propertyCache )
		: propertyCache_( propertyCache )
	{
	}
	std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() override
	{
		propertyCache_.emplace_back();
		return propertyCache_.back();
	}
private:
	RPURU::UndoRedoHelperList& propertyCache_;
};


/**
*	Function object which will iterate over elements in a cache so they
*	can be overwritten.
*/
class PropertyCacheIterator
	: public PropertyCacheFiller
{
public:
	PropertyCacheIterator( RPURU::UndoRedoHelperList & propertyCache )
		: propertyCache_( propertyCache )
		, itr_( propertyCache.begin() )
	{
	}

	std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() override
	{
		assert( itr_ != propertyCache_.end() );
		auto& helper = (*itr_);
		++itr_;
		return helper;
	}
private:
	RPURU::UndoRedoHelperList& propertyCache_;
	RPURU::UndoRedoHelperList::iterator itr_;
};


/**
*	Function pointer for getting either pre- or post-values from the
*	property cache.
*/
typedef Variant (*PropertyGetter)( const RPURU::ReflectedPropertyUndoRedoHelper& );


/**
*	Function pointer for setting either pre- or post-values on the
*	property cache.
*/
typedef void (*PropertySetter)( RPURU::ReflectedPropertyUndoRedoHelper&,
							   const Variant& );


/**
*	Function for getting undo data from the property cache.
*	@param helper the cache which contains the undo value.
*	@return the value from the cache.
*/
Variant undoPropertyGetter( const RPURU::ReflectedPropertyUndoRedoHelper& helper )
{
	return helper.preValue_;
}


/**
*	Function for getting redo data from the property cache.
*	@param helper the cache which contains the redo value.
*	@return the value from the cache.
*/
Variant redoPropertyGetter( const RPURU::ReflectedPropertyUndoRedoHelper& helper )
{
	return helper.postValue_;
}


/**
*	Function for setting undo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void undoPropertySetter( RPURU::ReflectedPropertyUndoRedoHelper& helper,
						const Variant& value )
{
	helper.preValue_ = value;
}


/**
*	Function for setting redo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void redoPropertySetter( RPURU::ReflectedPropertyUndoRedoHelper& helper,
						const Variant& value )
{
	helper.postValue_ = value;
}


void loadReflectedPropertyError(
	std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& helper,
	PropertySetter propertySetter,
	const std::string& message )
{
	auto propertyHelper = new RPURU::ReflectedPropertyUndoRedoHelper();
	helper.reset( propertyHelper );
	propertySetter( *propertyHelper, Variant( "Unknown" ) );
	std::string logMessage = "Failed to load reflected properties - " + message + "\n";
	NGT_TRACE_MSG( logMessage.c_str() );
}


//==============================================================================
bool loadReflectedProperties( PropertyCacheFiller & outPropertyCache,
							 IDataStream & stream,
							 PropertySetter propertySetter,
							 IObjectManager & objectManager,
							 IDefinitionManager & definitionManager )
{
	if (stream.eof())
	{
		return true;
	}

	auto pSerializationMgr = objectManager.getSerializationManager();
	assert( pSerializationMgr != nullptr );

	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
	const char * methodHeaderTag = RPURU::getMethodHeaderTag();

	while (!stream.eof())
	{
		auto& helper = outPropertyCache.getNext();

		// read header
		std::string header;
		stream.read( header );
		bool propertyHeader = header == propertyHeaderTag;
		bool methodHeader = header == methodHeaderTag;

		if (propertyHeader)
		{
			if (helper.get() == nullptr)
			{
				helper.reset( new RPURU::ReflectedPropertyUndoRedoHelper() );
			}
			else if (helper->isMethod())
			{
				loadReflectedPropertyError( helper, propertySetter, "invalid header");
				return true;
			}
		}
		else if (header == methodHeaderTag)
		{
			if (helper.get() == nullptr)
			{
				helper.reset( new RPURU::ReflectedMethodUndoRedoHelper() );
			}
			else if (!helper->isMethod())
			{
				loadReflectedPropertyError( helper, propertySetter, "invalid header");
				return true;
			}
		}
		else
		{
			loadReflectedPropertyError( helper, propertySetter, "invalid header");
			return true;
		}

		// read root object id
		std::string id;
		stream.read( id );

		if (id.empty())
		{
			loadReflectedPropertyError( helper, propertySetter, "invalid ID");
			return true;
		}

		helper->objectId_ = RefObjectId( id );
		
		// read property fullpath
		stream.read( helper->path_ );
		const auto& fullPath = helper->path_;

		ObjectHandle object = objectManager.getObject( helper->objectId_ );
		if (!object.isValid())
		{
			loadReflectedPropertyError( helper, propertySetter, "invalid object");
			return true;
		}

		PropertyAccessor pa = object.getDefinition( definitionManager )->bindProperty(
			fullPath.c_str(), object );

		if (!pa.isValid())
		{
			loadReflectedPropertyError( helper, propertySetter, "invalid property");
			return true;
		}

		if (propertyHeader)
		{
			// read value type
			std::string valueType;
			stream.read( valueType );
			// read value
			const MetaType * metaType = Variant::getMetaTypeManager()->findType( valueType.c_str() );

			if (metaType == nullptr)
			{
				loadReflectedPropertyError( helper, propertySetter, "invalid meta type");
				return true;
			}

			Variant value = pa.getValue();
			if (ReflectionUtilities::isStruct(pa))
			{
				if ( metaType != value.type() )
				{
					loadReflectedPropertyError( helper, propertySetter, "invalid value type");
					return true;
				}

				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>( helper.get() );

				pSerializationMgr->deserialize( stream, value );
				propertySetter( *propertyHelper, value );
			}
			else
			{
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>( helper.get() );

				Variant variant( metaType );
				pSerializationMgr->deserialize( stream, variant );
				propertySetter( *propertyHelper, variant );
			}
		}
		else
		{
			size_t parameterCount;
			stream.read( parameterCount );

			if (parameterCount > 10)
			{
				loadReflectedPropertyError( helper, propertySetter, "invalid number of method parameters");
				return true;
			}

			IMetaTypeManager* metaManager = Variant::getMetaTypeManager();
			auto methodHelper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>( helper.get() );

			while (parameterCount--)
			{
				std::string parameterType;
				stream.read( parameterType );
				const MetaType* metaType = metaManager->findType( parameterType.c_str() );

				if (metaType == nullptr)
				{
					loadReflectedPropertyError( helper, propertySetter, "invalid parameter type");
					return true;
				}

				Variant parameterValue( metaType );
				stream.read( parameterValue );
				methodHelper->parameters_.push_back( parameterValue );
			}
		}
	}

	return true;
}

//==============================================================================
void resolveProperty(
	const ObjectHandle & handle,
	const IClassDefinition & classDef,
	const char * propertyPath,
	PropertyAccessor & o_Pa,
	IDefinitionManager & definitionManager )
{
	o_Pa = handle.getDefinition( definitionManager )->bindProperty( propertyPath, handle );
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
			if ( (subHandle == nullptr) || (subHandle.getDefinition( definitionManager ) == nullptr) )
			{
				continue;
			}
			parentPath = parentPath + "." + propertyPath;
			resolveProperty( handle, *subHandle.getDefinition( definitionManager ), parentPath.c_str(), o_Pa, definitionManager );
			if(o_Pa.isValid())
			{
				return;
			}
		}
	}
}

//==============================================================================
bool applyReflectedProperties(
	const RPURU::UndoRedoHelperList & propertyCache,
	PropertyGetter propertyGetter,
	IObjectManager & objectManager,
	IDefinitionManager & definitionManager,
	bool undo )
{
	for (const auto& helper : propertyCache)
	{
		// read root object id
		const auto& id = helper->objectId_;

		// read property fullpath
		const auto& fullPath = helper->path_;

		ObjectHandle object = objectManager.getObject( id );
		if ( object == nullptr )
		{
			NGT_TRACE_MSG( "Failed to apply reflected property - object is null\n" );
			return false;
		}
		PropertyAccessor pa( object.getDefinition( definitionManager )->bindProperty( fullPath.c_str(), object ) );

		assert( pa.isValid() );

		if (helper->isMethod())
		{
			auto methodHelper = static_cast<const RPURU::ReflectedMethodUndoRedoHelper*>( helper.get() );

			if (undo)
			{
				ReflectedMethod* method = static_cast<ReflectedMethod*>( pa.getProperty() );
				method = method->getUndoMethod();
				assert( method != nullptr );
				method->invoke( object, methodHelper->parameters_ );
			}
			else
			{
				pa.invoke( methodHelper->parameters_ );
			}
		}
		else
		{
			auto propertyHelper = static_cast<const RPURU::ReflectedPropertyUndoRedoHelper*>( helper.get() );
			const auto& value = propertyGetter( *propertyHelper );
			bool br = pa.setValue( value );
			if (!br)
			{
				return false;
			}
		}
	}

	return true;
}

//==============================================================================
bool performReflectedUndoRedo( IDataStream& data,
							  PropertyGetter propertyGetter,
							  PropertySetter propertySetter,
							  const char* expectedFormatHeader,
							  IObjectManager & objectManager,
							  IDefinitionManager & definitionManager,
							  bool undo )
{
	data.seek( 0 );
	std::string formatHeader;
	data.read( formatHeader );
	assert( formatHeader == expectedFormatHeader );

	RPURU::UndoRedoHelperList propertyCache;
	PropertyCacheCreator creator( propertyCache );
	const bool loaded = loadReflectedProperties(
		creator,
		data,
		propertySetter,
		objectManager,
		definitionManager );
	const bool applied = applyReflectedProperties( propertyCache,
		propertyGetter,
		objectManager,
		definitionManager,
		undo );
	return (loaded && applied);
}

} //end namespace

//==============================================================================
const char * RPURU::getUndoStreamHeaderTag()
{
	static const char * s_ReflectionUndoData = "ReflectionUndoData";
	return s_ReflectionUndoData;
}

//==============================================================================
const char * RPURU::getRedoStreamHeaderTag()
{
	static const char * s_ReflectionRedoData = "ReflectionRedoData";
	return s_ReflectionRedoData;
}

//==============================================================================
const char * RPURU::getPropertyHeaderTag()
{
	static const char * s_ReflectionPropertyData = "ReflectionPropertyData";
	return s_ReflectionPropertyData;
}

//==============================================================================
const char * RPURU::getMethodHeaderTag()
{
	static const char * s_ReflectionMethodData = "ReflectionMethodData";
	return s_ReflectionMethodData;
}

//==============================================================================
bool RPURU::loadReflectedProperties( UndoRedoHelperList & outPropertyCache,
							 IDataStream & undoStream,
							 IDataStream & redoStream,
							 IObjectManager & objectManager,
							 IDefinitionManager & definitionManager )
{
	PropertyCacheCreator pcc( outPropertyCache );
	const bool undoSuccess = loadReflectedProperties(
		pcc,
		undoStream,
		&undoPropertySetter,
		objectManager,
		definitionManager );

	PropertyCacheIterator pci( outPropertyCache );
	const bool redoSuccess = loadReflectedProperties(
		pci,
		redoStream,
		&redoPropertySetter,
		objectManager,
		definitionManager );

	return (undoSuccess && redoSuccess);
}

//==============================================================================
std::string RPURU::resolveContextObjectPropertyPath(
	const ObjectHandle & contextObject, const char * propertyPath, IDefinitionManager & definitionManager )
{
	assert( contextObject != nullptr );
	const auto classDef = contextObject.getDefinition( definitionManager );
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
	PropertyAccessor pa;
	for (auto & path : paths)
	{
		resolveProperty( contextObject, *classDef, path.c_str(), pa, definitionManager );
		if (pa.isValid())
		{
			break;
		}
	}
	return pa.getFullPath();
}


bool RPURU::performReflectedUndo( IDataStream& data,
								 IObjectManager & objectManager,
								 IDefinitionManager & definitionManager )
{
	return performReflectedUndoRedo(
		data, &undoPropertyGetter, &undoPropertySetter, getUndoStreamHeaderTag(), objectManager, definitionManager, true );
}


bool RPURU::performReflectedRedo( IDataStream& data,
								 IObjectManager & objectManager,
								 IDefinitionManager & definitionManager )
{
	return performReflectedUndoRedo(
		data, &redoPropertyGetter, &redoPropertySetter, getRedoStreamHeaderTag(), objectManager, definitionManager, false );
}


void RPURU::saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream,
	const ReflectedClassMemberUndoRedoHelper& helper )
{
	if (helper.isMethod())
	{
		auto methodHelper = static_cast<const ReflectedMethodUndoRedoHelper*>( &helper );
		saveUndoData( serializationMgr, stream, *methodHelper );
	}
	else
	{
		auto propertyHelper = static_cast<const ReflectedPropertyUndoRedoHelper*>( &helper );
		saveUndoData( serializationMgr, stream, *propertyHelper );
	}
}


void RPURU::saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream,
	const ReflectedClassMemberUndoRedoHelper& helper )
{
	if (helper.isMethod())
	{
		auto methodHelper = static_cast<const ReflectedMethodUndoRedoHelper*>( &helper );
		saveRedoData( serializationMgr, stream, *methodHelper );
	}
	else
	{
		auto propertyHelper = static_cast<const ReflectedPropertyUndoRedoHelper*>( &helper );
		saveRedoData( serializationMgr, stream, *propertyHelper );
	}
}


void RPURU::saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream,
	const ReflectedPropertyUndoRedoHelper& helper )
{
	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
	//write header
	stream.write( propertyHeaderTag );
	// write root object id
	stream.write( helper.objectId_.toString() );
	// write property fullPath
	stream.write( helper.path_ );
	// write value type
	stream.write( helper.preValue_.type()->name() );
	// write value
	serializationMgr.serialize( stream, helper.preValue_ );
}


void RPURU::saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream, 
	const ReflectedPropertyUndoRedoHelper& helper )
{
	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
	//write header
	stream.write( propertyHeaderTag );
	// write root object id
	stream.write( helper.objectId_.toString() );
	// write property fullPath
	stream.write( helper.path_ );
	// write value type
	stream.write( helper.postValue_.type()->name() );
	// write value
	serializationMgr.serialize( stream, helper.postValue_ );
}


void RPURU::saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream, 
	const ReflectedMethodUndoRedoHelper& helper )
{
	const char* methodHeaderTag = RPURU::getMethodHeaderTag();
	stream.write( methodHeaderTag );
	stream.write( helper.objectId_.toString() );
	stream.write( helper.path_ );
	stream.write( helper.parameters_.size() );

	for (auto itr = helper.parameters_.cbegin(); itr != helper.parameters_.cend(); ++itr)
	{
		stream.write( itr->type()->name() );
		serializationMgr.serialize( stream, *itr );
	}
}


void RPURU::saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream, 
	const ReflectedMethodUndoRedoHelper& helper )
{
	const char* methodHeaderTag = RPURU::getMethodHeaderTag();
	stream.write( methodHeaderTag );
	stream.write( helper.objectId_.toString() );
	stream.write( helper.path_ );
	stream.write( helper.parameters_.size() );

	for (auto itr = helper.parameters_.cbegin(); itr != helper.parameters_.cend(); ++itr)
	{
		stream.write( itr->type()->name() );
		serializationMgr.serialize( stream, *itr );
	}
}
