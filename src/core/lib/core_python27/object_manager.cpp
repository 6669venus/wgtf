#include "pch.hpp"
#include "object_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/object_handle_storage_shared.hpp"


namespace ReflectedPython
{


ObjectManager::ObjectManager()
{
}


ObjectManager::~ObjectManager()
{
	assert( idMap_.empty() );
	assert( unmanagedMetaDataMap_.empty() );
}


ObjectHandle ObjectManager::getUnmanagedObject( const IClassDefinition & key ) const
{
	std::lock_guard< std::mutex > guard( objectsLock_ );
	auto findIt = unmanagedMetaDataMap_.find( &key );
	if (findIt == unmanagedMetaDataMap_.cend())
	{
		return nullptr;
	}
	return ObjectHandle( findIt->second );
}


RefObjectId ObjectManager::registerUnmanagedObject(
	const IClassDefinition & key,
	const ObjectHandle & handle,
	const RefObjectId & id )
{
	RefObjectId newId = id;
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );

		if( newId == RefObjectId::zero() )
		{
			newId = RefObjectId::generate();
		}

		auto metaData = std::make_shared< ObjectMetaData >();
		metaData->id_ = newId;
		metaData->handle_ = handle;
		metaData->deregistered_ = false;

		unmanagedMetaDataMap_.insert(
			std::make_pair( &key, metaData ) );

		auto insertResult = idMap_.insert( std::make_pair( newId, metaData ) );
		assert( insertResult.second );
	}
	//this->resolveObjectLink( newId, handle );
	//this->NotifyObjectRegistred( handle );

	return newId;
}


bool ObjectManager::deregisterUnmanagedObject( const IClassDefinition & key )
{
	ObjectHandle handle;
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );

		auto it = unmanagedMetaDataMap_.find( &key );
		if (it == unmanagedMetaDataMap_.end())
		{
			return false;
		}

		auto & metaData = it->second;
		handle = metaData;
		metaData->deregistered_ = true;
		idMap_.erase( metaData->id_ );
		unmanagedMetaDataMap_.erase( it );
	}

	//this->NotifyObjectDeregistred( handle );

	return true;
}


void ObjectManager::clear()
{
	assert( idMap_.empty() );
	assert( unmanagedMetaDataMap_.empty() );

	while (!unmanagedMetaDataMap_.empty())
	{
		this->deregisterUnmanagedObject( *(unmanagedMetaDataMap_.begin()->first) );
	}
	idMap_.clear();
}


} // namespace ReflectedPython

