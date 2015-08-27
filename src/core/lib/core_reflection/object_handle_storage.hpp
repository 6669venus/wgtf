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

void throwBaseProvider( const IClassDefinition* definition, const void* pointer );

void* castWithBaseProvider( const IClassDefinition* definition, const void* pointer, const TypeId & typeId );

void* castWithProvider( const void* pointer, const TypeId & typeId );


class IObjectHandleStorage
{
public:
	virtual ~IObjectHandleStorage() {}
	virtual bool isValid() const = 0;
	virtual bool tryCast( const TypeId & typeId ) const = 0;
	virtual void * castHelper( const TypeId & typeId ) const = 0;
	virtual void * getRaw() const = 0;
	virtual const IClassDefinition * getDefinition() const = 0;
	virtual void throwBase() const = 0;
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

	bool tryCast( const TypeId & typeId ) const override
	{
		static TypeId s_TypeId = TypeId::getType< T >();
		return typeId == s_TypeId;
	}

	void * castHelper( const TypeId & typeId ) const override
	{
		static TypeId s_TypeId = TypeId::getType< T >();

		if (typeId == s_TypeId)
		{
			return getRaw();
		}

		throwBase();
		//dummy return to suppress compiler warnings.
		return nullptr;
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


	void throwBase() const override
	{
		throw getPointer();
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


	void * castHelper( const TypeId & typeId ) const override
	{
		static TypeId s_TypeId = getType();

		if(typeId == s_TypeId)
		{
			return ObjectHandleStorageBase< T >::getRaw();
		}

		T * pointer = ObjectHandleStorageBase< T >::getPointer();
		return pointer == nullptr ? nullptr : castHelper( typeId, pointer );
	}


	virtual TypeId getType() const
	{
		return TypeId::getType< T >();
	}


	const IClassDefinition * getDefinition() const override
	{
		auto pointer = ObjectHandleStorageBase< T >::getPointer();
		return pointer == nullptr ? nullptr : getDefinition( pointer );
	}


	void throwBase() const override
	{
		auto pointer = ObjectHandleStorageBase< T >::getPointer();

		if (pointer == nullptr)
		{
			throw static_cast< T * >( nullptr );
		}

		//Get most derived type first before throwing
		auto * definition = getDefinition();

		if (definition == nullptr)
		{
			throw static_cast< T * >( nullptr );
		}

		throwBaseProvider( definition, pointer );
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

	template <class U>
	void * castHelper( const TypeId & typeId, const U* p ) const
	{
		T * pointer = ObjectHandleStorageBase< T >::getPointer();

		if (pointer == nullptr)
		{
			return nullptr;
		}

		return ObjectHandleStorageBase<T>( pointer, nullptr ).castHelper(typeId);
		//return castWithProvider( pointer, typeId );
	}

	template <class U>
	typename std::enable_if<std::is_pointer<U>::value>::type * castHelper( const TypeId & typeId, const U* p ) const
	{
		T * pointer = ObjectHandleStorageBase< T >::getPointer();
		if (pointer == nullptr)
		{
			return nullptr;
		}

		return ObjectHandleStoragePtr< std::remove_pointer<T> >( *pointer, nullptr ).castHelper(typeId);
	}

	void * castHelper( const TypeId & typeId, const ReflectedPolyStruct* pStruct ) const
	{
		auto definition = ::getPolyStructDefinition( pStruct );
		auto pointer = ObjectHandleStorageBase< T >::getPointer();
		
		return castWithBaseProvider( definition, pointer, typeId );
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


	TypeId getType() const override
	{
		return TypeId::getType< std::unique_ptr< T > >();
	}


	void * getRaw() const override
	{
		const void * pointer = const_cast< T * >( pointer_.get() );
		return const_cast< void * >( pointer );
	}


	T * getPointer() const override
	{
		return pointer_.get();
	}


private:
	std::unique_ptr< T > pointer_;
};

#endif // OBJECT_HANDLE_STORAGE_HPP
