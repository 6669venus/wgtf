#ifndef OBJECT_HANDLE_STORAGE_HPP
#define OBJECT_HANDLE_STORAGE_HPP

#include "core_variant/type_id.hpp"
#include "interfaces/i_class_definition.hpp"
#include <cassert>
#include <memory>
#include <unordered_set>
#include <type_traits>

class RefObjectId;
class ReflectedPolyStruct;

const IClassDefinition* getPolyStructDefinition( const ReflectedPolyStruct* polyStruct );
class IObjectHandleStorage
{
public:
	virtual ~IObjectHandleStorage() {}
	virtual bool isValid() const = 0;
	virtual void * getRaw() const = 0;
	virtual const IClassDefinition * getDefinition() const = 0;
	virtual bool getId( RefObjectId & id ) const = 0;
	virtual TypeId getPointedType() const = 0;
};


//==============================================================================
template< typename T >
class ObjectHandleStorageBase
	: public IObjectHandleStorage
{
public:
	ObjectHandleStorageBase(
		T * pointer, const IClassDefinition * definition )
		: definition_( definition )
		, pointer_( pointer )
	{}


	bool isValid() const override
	{
		return getPointer() != nullptr;
	}


	void * getRaw() const override
	{
		const void * pointer = const_cast< T * >( pointer_ );
		return const_cast< void * >( pointer );
	}


	virtual T * getPointer() const
	{
		return pointer_;
	}


	const IClassDefinition * getDefinition() const override
	{
		return getPointer() == nullptr ? nullptr : definition_;
	}


	bool getId( RefObjectId & id ) const override
	{
		return false;
	}


	TypeId getPointedType() const override
	{
		return TypeId::getType< T >();
	}

private:
	const IClassDefinition * definition_;
	T * pointer_;
};


//==============================================================================
template< typename T >
class ObjectHandleStoragePtr
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStoragePtr(
		T * pointer, const IClassDefinition * definition )
		: ObjectHandleStorageBase< T >( pointer, definition )
	{
	}


	const IClassDefinition * getDefinition() const override
	{
		auto pointer = ObjectHandleStorageBase< T >::getPointer();
		return pointer == nullptr ? nullptr : getDefinition( pointer );
	}


private:
	const IClassDefinition * getDefinition(
		const ReflectedPolyStruct * polyStruct ) const
	{
		return ::getPolyStructDefinition( polyStruct );
	}


	const IClassDefinition * getDefinition( const void * ) const
	{
		return ObjectHandleStorageBase<T>::getDefinition();
	}
};


//==============================================================================
template< typename T >
class ObjectHandleStorageCopy
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStorageCopy( T & temp, const IClassDefinition * definition )
		: ObjectHandleStorageBase< T >( &storage_, definition )
		, storage_( temp )
	{}


private:
	T storage_;
};


//==============================================================================
template< typename T >
class ObjectHandleStorage
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStorage( T & object, const IClassDefinition * definition )
		: ObjectHandleStorageBase< T >( &object, definition )
	{}
};


//==============================================================================
template< typename T >
class ObjectHandleStorage< T * >
	: public ObjectHandleStoragePtr< T >
{
public:
	ObjectHandleStorage( T *& pointer, const IClassDefinition * definition )
		: ObjectHandleStoragePtr< T >( pointer, definition )
	{}
};


//==============================================================================
template< typename T >
class ObjectHandleStorage< std::unique_ptr< T > >
	: public ObjectHandleStoragePtr< T >
{
public:
	ObjectHandleStorage(
		std::unique_ptr< T > && pointer,
		const IClassDefinition * definition = nullptr )
		: ObjectHandleStoragePtr<T>( pointer.get(), definition )
		, pointer_( std::move( pointer ) )
	{}


private:
	const std::unique_ptr< T > pointer_;
};

#endif // OBJECT_HANDLE_STORAGE_HPP
