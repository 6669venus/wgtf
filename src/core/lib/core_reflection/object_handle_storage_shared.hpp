#ifndef OBJECT_HANDLE_STORAGE_SHARED_HPP
#define OBJECT_HANDLE_STORAGE_SHARED_HPP

#include "object_handle_storage.hpp"
#include "object_handle.hpp"

struct ObjectMetaData;
typedef std::unordered_set< ObjectMetaData * > ObjIdSet;

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
	bool tryCast( const TypeId & typeId ) const override
	{
		if(metaData_->handle_ == nullptr)
		{
			return false;
		}
		return metaData_->handle_.getStorage()->tryCast( typeId );
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


	//--------------------------------------------------------------------------
	TypeId getPointedType() const override
	{
		return TypeId::getType< ObjectMetaData >();
	}

private:
	std::shared_ptr< ObjectMetaData > metaData_;
};

#endif // OBJECT_HANDLE_STORAGE_SHARED_HPP
