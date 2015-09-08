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
	virtual void * data() const = 0;
	virtual TypeId type() const = 0;

	virtual std::shared_ptr< IObjectHandleStorage > inner() const { return nullptr; }

	virtual bool getId( RefObjectId & id ) const = 0;
	virtual const IClassDefinition * getDefinition( const IDefinitionManager & definitionManager ) const = 0;
	virtual void throwBase() const = 0;
};


//==============================================================================
template< typename T >
class ObjectHandleStorageBase
	: public IObjectHandleStorage
{
public:
	ObjectHandleStorageBase( const IClassDefinition * definition = nullptr )
		: definition_( definition )
	{}


	virtual T * getPointer() const = 0;


	void * data() const override
	{
		const void * data = getPointer();
		return const_cast< void * >( data );
	}


	TypeId type() const override
	{
		static TypeId s_Type = TypeId::getType< T >();
		return s_Type;
	}


	bool getId( RefObjectId & id ) const override
	{
		return false;
	}


	const IClassDefinition * getDefinition( const IDefinitionManager & definitionManager ) const override
	{
		return getDefinition( getPointer(), definitionManager );
	}


	void throwBase() const override
	{
		throw getPointer();
	}


private:
	const IClassDefinition * getDefinition(
		const ReflectedPolyStruct * polyStruct,
		const IDefinitionManager & definitionManager ) const
	{
		return polyStruct == nullptr ? nullptr : ::getPolyStructDefinition( polyStruct );
	}


	const IClassDefinition * getDefinition( const void *, const IDefinitionManager & definitionManager ) const
	{
		return definition_;
	}


	const IClassDefinition * definition_;
};


//==============================================================================
template< typename T >
class ObjectHandleStoragePtr
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStoragePtr(
		T * pointer, const IClassDefinition * definition )
		: ObjectHandleStorageBase< T >( definition )
		, pointer_( pointer )
	{
	}


	T * getPointer() const override
	{
		return pointer_;
	}


private:
	T * pointer_;
};


//==============================================================================
template< typename T >
class ObjectHandleStorage
	: public ObjectHandleStoragePtr< T >
{
public:
	ObjectHandleStorage( T & temp, const IClassDefinition * definition )
		: ObjectHandleStoragePtr< T >( &storage_, definition )
		, storage_( temp )
	{}


private:
	T storage_;
};


//==============================================================================
template< typename T >
class ObjectHandleStorage< T * >
	: public ObjectHandleStoragePtr< T >
{
public:
	ObjectHandleStorage( T * pointer, const IClassDefinition * definition )
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


//==============================================================================
template< typename T >
class ObjectHandleStorageReinterpretCast
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStorageReinterpretCast(
		const std::shared_ptr< IObjectHandleStorage > & storage )
		: storage_( storage )
	{
	}


	T * getPointer() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		assert( storage_->type() == TypeId::getType< T >() );
		return reinterpret_cast< T * >( storage_->data() );
	}


	std::shared_ptr< IObjectHandleStorage > inner() const
	{
		return storage_;
	}


	bool getId( RefObjectId & id ) const override
	{
		return storage_->getId( id );
	}

private:
	std::shared_ptr< IObjectHandleStorage > storage_;
};


//==============================================================================
template< typename T, typename T2 >
class ObjectHandleStorageStaticCast
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStorageStaticCast( const std::shared_ptr< ObjectHandleStorageBase< T2 > > & storage )
		: storage_( storage )
	{
	}


	T * getPointer() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		return storage_->getPointer();
	}


	std::shared_ptr< IObjectHandleStorage > inner() const
	{
		return storage_;
	}


	bool getId( RefObjectId & id ) const override
	{
		return storage_->getId( id );
	}

private:
	std::shared_ptr< ObjectHandleStorageBase< T2 > > storage_;
};


//==============================================================================
template< typename T >
class ObjectHandleStorageReflectedCast
	: public ObjectHandleStorageBase< T >
{
public:
	ObjectHandleStorageReflectedCast(
		const std::shared_ptr< IObjectHandleStorage > & storage,
		const IDefinitionManager & definitionManager )
		: storage_( storage )
		, definitionManager_( definitionManager )
	{
	}


	T * getPointer() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		return reinterpret_cast< T * >( reflectedCast( 
			storage_->data(), 
			storage_->type(),
			TypeId::getType< T >(), 
			definitionManager_ ) );
	}


	std::shared_ptr< IObjectHandleStorage > inner() const
	{
		return storage_;
	}


	bool getId( RefObjectId & id ) const override
	{
		return storage_->getId( id );
	}

private:
	std::shared_ptr< IObjectHandleStorage > storage_;
	const IDefinitionManager & definitionManager_;
};

#endif // OBJECT_HANDLE_STORAGE_HPP
