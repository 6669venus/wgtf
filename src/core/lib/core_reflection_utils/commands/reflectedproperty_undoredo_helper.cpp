#include "reflectedproperty_undoredo_helper.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
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
	virtual RPURU::ReflectionPropertyUndoRedoHelper& getNext() = 0;
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
	RPURU::ReflectionPropertyUndoRedoHelper& getNext() override
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

	RPURU::ReflectionPropertyUndoRedoHelper& getNext() override
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
typedef Variant (*PropertyGetter)( const RPURU::ReflectionPropertyUndoRedoHelper& );


/**
*	Function pointer for setting either pre- or post-values on the
*	property cache.
*/
typedef void (*PropertySetter)( RPURU::ReflectionPropertyUndoRedoHelper&,
							   const Variant& );


/**
*	Function for getting undo data from the property cache.
*	@param helper the cache which contains the undo value.
*	@return the value from the cache.
*/
Variant undoPropertyGetter( const RPURU::ReflectionPropertyUndoRedoHelper& helper )
{
	return helper.preValue_;
}


/**
*	Function for getting redo data from the property cache.
*	@param helper the cache which contains the redo value.
*	@return the value from the cache.
*/
Variant redoPropertyGetter( const RPURU::ReflectionPropertyUndoRedoHelper& helper )
{
	return helper.postValue_;
}


/**
*	Function for setting undo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void undoPropertySetter( RPURU::ReflectionPropertyUndoRedoHelper& helper,
						const Variant& value )
{
	helper.preValue_ = value;
}


/**
*	Function for setting redo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void redoPropertySetter( RPURU::ReflectionPropertyUndoRedoHelper& helper,
						const Variant& value )
{
	helper.postValue_ = value;
}


//==============================================================================
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
	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
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
			pSerializationMgr->deserialize( stream, variant );
			propertySetter( helper, variant );
		}
	}

	return true;
}

//==============================================================================
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

//==============================================================================
bool applyReflectedProperties( const RPURU::UndoRedoHelperList & propertyCache,
							  PropertyGetter propertyGetter,
							  IObjectManager & objectManager )
{
	for (const auto& helper : propertyCache)
	{
		// read root object id
		const auto& id = helper.objectId_;

		// read property fullpath
		const auto& fullPath = helper.propertyPath_;

		ObjectHandle object = objectManager.getObject( id );
		if ( object == nullptr )
		{
			NGT_TRACE_MSG( "Failed to apply reflected property - object is null\n" );
			return false;
		}
		PropertyAccessor pa( object.getDefinition()->bindProperty( fullPath.c_str(), object ) );

		assert( pa.isValid() );

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

//==============================================================================
bool performReflectedUndoRedo( IDataStream& data,
							  PropertyGetter propertyGetter,
							  PropertySetter propertySetter,
							  const char* expectedFormatHeader,
							  IObjectManager & objectManager )
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
		objectManager );
	const bool applied = applyReflectedProperties( propertyCache,
		propertyGetter,
		objectManager );
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
bool RPURU::loadReflectedProperties( UndoRedoHelperList & outPropertyCache,
							 IDataStream & undoStream,
							 IDataStream & redoStream,
							 IObjectManager & objectManager )
{
	PropertyCacheCreator pcc( outPropertyCache );
	const bool undoSuccess = loadReflectedProperties(
		pcc,
		undoStream,
		&undoPropertySetter,
		objectManager );

	PropertyCacheIterator pci( outPropertyCache );
	const bool redoSuccess = loadReflectedProperties(
		pci,
		redoStream,
		&redoPropertySetter,
		objectManager );

	return (undoSuccess && redoSuccess);
}

//==============================================================================
std::string RPURU::resolveContextObjectPropertyPath(
	const ObjectHandle & contextObject, const char * propertyPath )
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
	PropertyAccessor pa;
	for (auto & path : paths)
	{
		resolveProperty( contextObject, *classDef, path.c_str(), pa );
		if (pa.isValid())
		{
			break;
		}
	}
	return pa.getFullPath();
}

bool RPURU::performReflectedUndo( IDataStream& data,
								 IObjectManager & objectManager )
{
	return performReflectedUndoRedo( data,
				&undoPropertyGetter,
				&undoPropertySetter,
				getUndoStreamHeaderTag(),
				objectManager );
}

bool RPURU::performReflectedRedo( IDataStream& data,
								 IObjectManager & objectManager )
{
	return performReflectedUndoRedo( data,
				&redoPropertyGetter,
				&redoPropertySetter,
				getRedoStreamHeaderTag(),
				objectManager );
}


void RPURU::saveUndoData( ISerializationManager & serializationMgr,
						 IDataStream & stream,
						 const ReflectionPropertyUndoRedoHelper& helper )
{
	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
	//write header
	stream.write( propertyHeaderTag );
	// write root object id
	stream.write( helper.objectId_.toString() );
	// write property fullPath
	stream.write( helper.propertyPath_ );
	// write value type
	stream.write( helper.preValue_.type()->name() );
	// write value
	serializationMgr.serialize( stream, helper.preValue_ );

}

void RPURU::saveRedoData( ISerializationManager & serializationMgr,
						  IDataStream & stream,
						  const ReflectionPropertyUndoRedoHelper& helper )
{
	const char * propertyHeaderTag = RPURU::getPropertyHeaderTag();
	//write header
	stream.write( propertyHeaderTag );
	// write root object id
	stream.write( helper.objectId_.toString() );
	// write property fullPath
	stream.write( helper.propertyPath_ );
	// write value type
	stream.write( helper.postValue_.type()->name() );
	// write value
	serializationMgr.serialize( stream, helper.postValue_ );

}
