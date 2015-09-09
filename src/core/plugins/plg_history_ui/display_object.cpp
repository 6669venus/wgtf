#include "display_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"

namespace RPURU = ReflectedPropertyUndoRedoUtility;

//==============================================================================
DisplayObject::DisplayObject()
	: data_( nullptr )
{
}

//==============================================================================
void DisplayObject::init( IDefinitionManager & defManager, const CommandInstancePtr & instance )
{
		assert( instance != nullptr );
		auto pObjectManager = defManager.getObjectManager();
		assert( pObjectManager != nullptr );
		auto& objectManager = (*pObjectManager);
		const char * undoStreamHeaderTag = RPURU::getUndoStreamHeaderTag();
		const char * redoStreamHeaderTag = RPURU::getRedoStreamHeaderTag();
		const IDataStream & undoData = instance->getUndoStream();
		const IDataStream & redoData = instance->getRedoStream();
		RPURU::UndoRedoHelperList propertyCache;
		{
			// Need to read undo/redo data separately and then consolidate it into
			// propertyCache.
	
			// Make a copy because this function should not modify stream contents
			// TODO ResizingMemoryStream const read implementation
			ResizingMemoryStream undoStream(
				static_cast< const char* >( undoData.rawBuffer() ),
				undoData.size() );
			assert( !undoStream.eof() );
	
			// Read property header
			std::string undoHeader;
			undoHeader.reserve( strlen( undoStreamHeaderTag ) );
			undoStream.read( undoHeader );
			assert( undoHeader == undoStreamHeaderTag );
	
			// Make a copy because this function should not modify stream contents
			// TODO ResizingMemoryStream const read implementation
			ResizingMemoryStream redoStream(
				static_cast< const char* >( redoData.rawBuffer() ),
				redoData.size() );
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
				return;
			}
		}
	
		// Create display object from cache
		{
	
			auto handle = GenericObject::create( defManager );
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
				auto objectMgr = defManager.getObjectManager();
				ObjectHandle object = objectManager.getObject( helper.objectId_ );
				assert ( object != nullptr );
				PropertyAccessor pa( object.getDefinition()->bindProperty( helper.propertyPath_.c_str(), object ) );
				auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >( pa );
				if (metaData != nullptr)
				{
					const char * propName = metaData->getPropertyName();
					pa = object.getDefinition()->bindProperty( propName, object );
					auto value = pa.getValue();
					std::string name;
					bool isOk = value.tryCast( name );
					assert( isOk );
					genericObject.set( "Name", name );
				}
				else
				{
					genericObject.set( "Name", helper.propertyPath_ );
				}
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
					auto childHandle = GenericObject::create( defManager );
					assert( childHandle.get() != nullptr );
	
					auto& childObject = (*childHandle);
					childObject.set( "Id", helper.objectId_ );
					auto objectMgr = defManager.getObjectManager();
					ObjectHandle object = objectManager.getObject( helper.objectId_ );
					assert ( object != nullptr );
					PropertyAccessor pa( object.getDefinition()->bindProperty( helper.propertyPath_.c_str(), object ) );
					auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >( pa );
					if (metaData != nullptr)
					{
						const char * propName = metaData->getPropertyName();
						pa = object.getDefinition()->bindProperty( propName, object );
						auto value = pa.getValue();
						std::string name;
						bool isOk = value.tryCast( name );
						assert( isOk );
						childObject.set( "Name", name );
					}
					else
					{
						childObject.set( "Name", helper.propertyPath_ );
					}
					childObject.set( "Type", helper.propertyTypeName_ );
					childObject.set( "PreValue", helper.preValue_ );
					childObject.set( "PostValue", helper.postValue_ );
	
					children.push_back( childHandle );
				}
	
				// Convert CollectionHolder to Collection
				Collection childrenCollection( collectionHolder );
				genericObject.set( "Children", childrenCollection );
			}
	
			data_ = ObjectHandle( std::move( handle ) );
		}
}

//==============================================================================
ObjectHandle DisplayObject::getDisplayData() const
{
	return data_;
}

