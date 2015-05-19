#include "object_manager.hpp"

#include "property_accessor.hpp"
#include "metadata/meta_types.hpp"
#include "metadata/meta_impl.hpp"
#include "metadata/meta_utilities.hpp"
#include "serialization/i_datastream.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"

#include <atomic>
#include <cassert>
#include <mutex>

namespace
{
	ObjectManager * s_Manager = nullptr;
}


struct ObjectMetaData
{
	bool						deregistered_;
	std::weak_ptr< ObjIdSet >	context_;
	ObjectHandle				handle_;
	RefObjectId					id_;
};

//==============================================================================
template<>
class ObjectHandleStorage< std::shared_ptr< ObjectMetaData > >
	: public IObjectHandleStorage
{
public:
	//--------------------------------------------------------------------------
	ObjectHandleStorage(
		std::shared_ptr< ObjectMetaData > & metaData,
		const IClassDefinition * definition = nullptr )
		: metaData_( metaData )
	{
	}


	//--------------------------------------------------------------------------
	const IClassDefinition * getDefinition() const
	{
		return metaData_->handle_.getDefinition();
	}


	//--------------------------------------------------------------------------
	void * castHelper( const TypeId & typeId ) const override
	{
		if(metaData_->handle_ == nullptr)
		{
			return nullptr;
		}
		return metaData_->handle_.getStorage()->castHelper( typeId );
	}


	//--------------------------------------------------------------------------
	void throwBase() const override
	{
		metaData_->handle_.throwBase();
	}


	//--------------------------------------------------------------------------
	void * getRaw() const override
	{
		auto & handle = metaData_->handle_;
		if(handle.isValid())
		{
			return handle.getStorage()->getRaw();
		}
		return nullptr;
	}


	//--------------------------------------------------------------------------
	bool isValid() const override
	{
		return metaData_->handle_ != nullptr;
	}


	//--------------------------------------------------------------------------
	bool getId( RefObjectId & o_Id ) const override
	{
		o_Id = metaData_->id_;
		return true;
	}
private:
	std::shared_ptr< ObjectMetaData > metaData_;
};


//==============================================================================
ObjectManager::ObjectManager()
	: pDefManager_( NULL )
	, pSerializationManager_( NULL )
{
	s_Manager = this;
}


//------------------------------------------------------------------------------
ObjectManager::~ObjectManager()
{
	objLink_.clear();
	std::vector< IDefinitionManager * > contexts; 
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );
		for (ContextObjects::iterator it = contextObjects_.begin(), 
			end = contextObjects_.end(); it != end; ++it)
		{
			contexts.push_back( it->first );
		}
	}

	for (auto it = contexts.begin(), end = contexts.end(); it != end; ++it)
	{
		deregisterContext( *it );
	}
	s_Manager = nullptr;
}


//------------------------------------------------------------------------------
void ObjectManager::init( IDefinitionManager * pDefManager )
{
	assert( pDefManager );
	pDefManager_ = pDefManager;
}


//==============================================================================
void ObjectManager::setSerializationManager(ISerializationManager * pSerilizationMgr)
{
	pSerializationManager_ = pSerilizationMgr;
}


//==============================================================================
ObjectHandle ObjectManager::getObject( const RefObjectId & id ) const
{
	std::lock_guard< std::mutex > guard( objectsLock_ );

	auto findIt = idMap_.find( id );
	if (findIt == idMap_.end())
	{
		return nullptr;
	}
	return ObjectHandle( findIt->second.lock() );
}


//------------------------------------------------------------------------------
ObjectHandle ObjectManager::getObject( const void * pObj ) const
{
	std::lock_guard< std::mutex > guard( objectsLock_ );
	auto findIt = metaDataMap_.find( pObj );
	if (findIt == metaDataMap_.end())
	{
		return nullptr;
	}
	return ObjectHandle( findIt->second.lock() );
}


//------------------------------------------------------------------------------
bool ObjectManager::getContextObjects( IDefinitionManager * context,
									  std::vector<const RefObjectId>& o_objects ) const
{
	std::lock_guard< std::mutex > guard( objectsLock_ );
	ContextObjects::const_iterator findIt = contextObjects_.find( context );
	assert( findIt != contextObjects_.end() );
	if (findIt == contextObjects_.end())
	{
		return false;
	}
	for (const auto & it : *findIt->second )
	{
		o_objects.push_back( it->id_ );
	}
	return true;
}

//------------------------------------------------------------------------------
void ObjectManager::getObjects( std::vector< const ObjectHandle > & o_objects ) const
{
	std::lock_guard< std::mutex > guard( objectsLock_ );
	o_objects.reserve(metaDataMap_.size());
	for ( const auto& it : metaDataMap_ )
	{
		auto pObj = it.second.lock();
		o_objects.push_back( pObj );
	}
}


//------------------------------------------------------------------------------
ObjectHandle ObjectManager::registerObject(
	ObjectHandle & handle, const RefObjectId& id )
{
	RefObjectId newId = id;
	ObjectHandle returnHandle;
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );
		
		if( newId == RefObjectId::zero() )
		{
			newId = RefObjectId::generate();
		}
		const auto cdef = handle.getDefinition();
		IDefinitionManager * pDefMgr = cdef->getDefinitionManager();
		assert( pDefMgr );


		auto & metaData =
			std::shared_ptr< ObjectMetaData >( new ObjectMetaData, [=]( ObjectMetaData * metaData )
		{
			std::unique_ptr< ObjectMetaData > holder( metaData );
			if (metaData->handle_.isValid() == false)
			{
				assert( metaData->deregistered_ );
				return;
			}
			deregisterMetaData( *metaData );
		});
		metaData->id_ = newId;
		metaData->handle_ = handle;
		metaData->deregistered_ = false;

		metaDataMap_.insert(
			std::make_pair( handle.getStorage()->getRaw(), metaData ) );

		auto contextIt = contextObjects_.find( pDefMgr );
		std::shared_ptr< ObjIdSet > objSet;
		if(contextIt == contextObjects_.end())
		{
			objSet = std::make_shared< ObjIdSet >();
			contextObjects_.insert( std::make_pair( pDefMgr, objSet ) );
		}
		else
		{
			objSet = contextIt->second;
		}
		objSet->insert( metaData.get() );
		metaData->context_ = objSet;
		auto insertResult = idMap_.insert( std::make_pair( newId, metaData ) );
		assert( insertResult.second );
		returnHandle = ObjectHandle( metaData );
	}
	resolveObjectLink( newId, returnHandle );
	NotifyObjectRegistred(returnHandle);

	return returnHandle;
}


//------------------------------------------------------------------------------
ObjectHandle ObjectManager::createObject( 
	const RefObjectId & id, 
	const std::string & classDef ) const
{
	assert( pDefManager_ );
	if ( !pDefManager_)
	{
		return nullptr;
	}

	auto pObj = getObject( id );
	if (pObj != nullptr)
	{
		assert( classDef.empty() || 
			classDef == pObj.getDefinition()->getName() );
		return pObj;
	}

	const auto pClassDef = 
		pDefManager_->getDefinition( classDef.c_str() );
	if (!pClassDef)
	{
		return nullptr;
	}

	pObj = pClassDef->createManagedObject( id );
	assert( pObj != nullptr );

	return pObj;
}


//------------------------------------------------------------------------------
ObjectHandle ObjectManager::create( 
	const std::string & classDef ) const
{
	assert( !classDef.empty() );
	assert( pDefManager_ );
	if ( !pDefManager_)
	{
		return NULL;
	}

	const auto pClassDef = 
		pDefManager_->getDefinition( classDef.c_str() );
	assert( pClassDef );
	if (!pClassDef)
	{
		return NULL;
	}

	return pClassDef->create();
}


//------------------------------------------------------------------------------
bool ObjectManager::registerContext( IDefinitionManager * context )
{
	std::lock_guard< std::mutex > guard( objectsLock_ );
	auto insertIt = contextObjects_.insert(
		std::make_pair( context, std::make_shared< ObjIdSet >() ) );
	assert( insertIt.second );
	return insertIt.second;
}


//------------------------------------------------------------------------------
bool ObjectManager::deregisterContext( IDefinitionManager * context )
{
	std::shared_ptr< ObjIdSet > copy;
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );

		auto & findIt = contextObjects_.find( context );
		assert( findIt != contextObjects_.end() );
		if (findIt == contextObjects_.end())
		{
			return false;
		}

		copy.swap( findIt->second );
		contextObjects_.erase( findIt );
	}

	for (auto & it = copy->begin(); it != copy->end(); ++it)
	{
		auto & metaData = *it;
		ObjectHandle & handle =  metaData->handle_;
		assert( handle.isValid() );
		RefObjectId id;
		{
			std::lock_guard< std::mutex > guard( objectsLock_ );
			id = metaData->id_;
			idMap_.erase( id );
			metaDataMap_.erase( handle.getStorage()->getRaw() );
		}
		metaData->deregistered_ = true;

		NotifyObjectDeregistred( handle );
		handle = nullptr;
	}
	return true;
}


//------------------------------------------------------------------------------
void ObjectManager::registerListener( IObjectManagerListener * listener )
{
	std::lock_guard< std::mutex > lisGuard( listenersLock_ );
	assert( std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
	listeners_.push_back(listener);
}


//------------------------------------------------------------------------------
void ObjectManager::deregisterListener( IObjectManagerListener * listener )
{
	std::lock_guard< std::mutex > lisGuard( listenersLock_ );
	ObjectManagerListener::iterator it = std::find(listeners_.begin(), listeners_.end(), listener);
	assert( it != listeners_.end());
	listeners_.erase(it);
}


//------------------------------------------------------------------------------
ISerializationManager * ObjectManager::getSerializationManager()
{
	return pSerializationManager_;
}


const ISerializationManager * ObjectManager::getSerializationManager() const
{
	return pSerializationManager_;
}


//------------------------------------------------------------------------------
void ObjectManager::deregisterMetaData(
	ObjectMetaData & metaData )
{
	const ObjectHandle & handle =  metaData.handle_;

	std::lock_guard< std::mutex > guard( objectsLock_ );

	assert(handle.isValid());

	RefObjectId id = metaData.id_;
	auto numFound = idMap_.erase( id );
	assert( numFound == 1 );
	auto context = metaData.context_.lock();
	assert(context != nullptr);
	numFound = context->erase( &metaData );
	assert( numFound == 1 );
	numFound = metaDataMap_.erase( handle.getStorage()->getRaw() );
	assert( numFound == 1 );

	metaData.deregistered_ = true;

	NotifyObjectDeregistred( handle );
}


//------------------------------------------------------------------------------
bool ObjectManager::saveObjects( IDataStream& dataStream, IDefinitionManager & defManager )
{
	assert( pSerializationManager_ );
	bool br = false;

	defManager.serializeDefinitions( dataStream );

	std::vector< const RefObjectId > objIdList;
	br = getContextObjects( &defManager, objIdList );
	assert( br );

	std::vector< ObjectHandle > objects;
	for(auto objid : objIdList)
	{
		auto pObj = getObject( objid );
		const auto & classDef = pObj.getDefinition();
		auto metaData = findFirstMetaData<MetaNoSerializationObj>( *classDef );
		if(metaData != nullptr)
		{
			continue;
		}
		assert( pObj != nullptr );
		objects.push_back( pObj );
		assert( br );
	}

	size_t count = objects.size();
	br = dataStream.write( count );
	for(auto obj : objects)
	{
		br = pSerializationManager_->serialize( dataStream, obj );
		assert( br );
	}
	return br;
}


//------------------------------------------------------------------------------
bool ObjectManager::loadObjects( IDataStream& dataStream, IDefinitionManager & defManager )
{
	assert( pSerializationManager_ );
	bool br = false;
	defManager.deserializeDefinitions( dataStream );
	size_t objCount = 0;
	br = dataStream.read( objCount );
	for(int j = 0; (j < objCount) && br; j++)
	{
		Variant variant = ObjectHandle();
		br = pSerializationManager_->deserialize( dataStream, variant );
		assert( br );
	}
	return br;
}


//------------------------------------------------------------------------------
void ObjectManager::addObjectLinks(
	const std::string & objId, PropertyAccessor & pa )
{
	std::lock_guard< std::mutex > objGuard( objLinkLock_ );
	objLink_.insert( std::make_pair( objId, std::move( pa ) ) );
}


//------------------------------------------------------------------------------
void ObjectManager::resolveObjectLink( const RefObjectId & objId, ObjectHandle object )
{
	std::lock_guard< std::mutex > objGuard( objLinkLock_ );
	auto findIt = objLink_.find( objId );
	if(findIt != objLink_.end())
	{
		auto & rootObject = findIt->second.getRootObject();
		rootObject.getDefinition()->bindProperty( 
			findIt->second.getFullPath(),
			rootObject );
		findIt->second.setValue( object );
		objLink_.erase( findIt );
	}
}

void ObjectManager::NotifyObjectRegistred(const ObjectHandle & handle) const
{
	std::lock_guard< std::mutex > lisGuard( listenersLock_ );
	for (auto& it : listeners_)
	{
		it->onObjectRegistered( handle );
	}
}

void ObjectManager::NotifyObjectDeregistred(const ObjectHandle & handle) const
{
	std::lock_guard< std::mutex > lisGuard( listenersLock_ );
	for (auto& it : listeners_)
	{
		it->onObjectDeregistered( handle );
	}
}

