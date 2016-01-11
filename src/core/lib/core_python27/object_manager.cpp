#include "pch.hpp"
#include "object_manager.hpp"
#include "core_logging/logging.hpp"
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
	return ObjectHandle( findIt->second.lock() );
}


ObjectHandle ObjectManager::registerUnmanagedObject(
	const IClassDefinition & key,
	const ObjectHandle & handle,
	const RefObjectId & id )
{
	const RefObjectId newId = (id == RefObjectId::zero()) ?
		RefObjectId::generate() :
		id;

	std::shared_ptr< ObjectMetaData > metaData( nullptr );
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );

		// Create strong ptr
		metaData = std::make_shared< ObjectMetaData >();
		metaData->id_ = newId;
		metaData->handle_ = handle;
		metaData->deregistered_ = false;

		// Store weak ptr
		auto itr = unmanagedMetaDataMap_.insert(
			std::make_pair( &key, metaData ) );

		auto insertResult = idMap_.insert( std::make_pair( newId, metaData ) );
		assert( insertResult.second );
	}

	// Return strong ptr
	return metaData;
}


bool ObjectManager::deregisterUnmanagedObject( const IClassDefinition & key )
{
	{
		std::lock_guard< std::mutex > guard( objectsLock_ );

		auto metaIt = unmanagedMetaDataMap_.find( &key );
		if (metaIt == unmanagedMetaDataMap_.end())
		{
			return false;
		}

		if (!metaIt->second.expired())
		{
			auto metaData = metaIt->second.lock();
			//handle = metaData;
			metaData->deregistered_ = true;
			idMap_.erase( metaData->id_ );
		}
		else
		{
			for (auto idItr = idMap_.cbegin(); idItr != idMap_.cend(); ++idItr)
			{
				if (idItr->second.expired())
				{
					idMap_.erase( idItr );
					break;
				}
			}
		}
		unmanagedMetaDataMap_.erase( metaIt );
	}

	return true;
}


void ObjectManager::clear()
{
	if (!unmanagedMetaDataMap_.empty())
	{
#if defined( _MSC_VER )
		NGT_ERROR_MSG( "%Iu unmanaged objects leaked\n", unmanagedMetaDataMap_.size() );
#else // defined( WIN32 )
		NGT_ERROR_MSG( "%zu unmanaged objects leaked\n", unmanagedMetaDataMap_.size() );
#endif // defined( WIN32 )
	}
	assert( unmanagedMetaDataMap_.empty() );
	assert( idMap_.empty() );

	while (!unmanagedMetaDataMap_.empty())
	{
		this->deregisterUnmanagedObject( *(unmanagedMetaDataMap_.begin()->first) );
	}
}


} // namespace ReflectedPython

