#ifndef OBJECT_HANDLE_HPP
#define OBJECT_HANDLE_HPP

/*
An ObjectHandle contains a data type and its associated ClassDefintion.
ObjectHandles store reflection data at runtime and are flexible enough to store
most types, including references, pointers and structures.
Use an ObjectHandle when a model or property needs to be exposed to QML.
Also use when working with reflection containers or reflected objects.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "variant/type_id.hpp"
#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition.hpp"
#include <cassert>
#include <memory>
#include <map>
#include <unordered_map>

class ReflectedPolyStruct;
class IClassDefinition;
class RefObjectId;

class ObjectHandle;

template< typename T >
class ObjectHandleT;


//==============================================================================
class IObjectHandleStorage
{
public:
	virtual ~IObjectHandleStorage() {}
	virtual bool isValid() const = 0;
	virtual void * castHelper( const TypeId & typeId ) const = 0;
	virtual void * getRaw() const = 0;
	virtual const IClassDefinition * getDefinition() const = 0;
	virtual void throwBase() const = 0;
	virtual bool getId( RefObjectId & id ) const = 0;
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
			return getRaw();
		}

		T * pointer = getPointer();
		return pointer == nullptr ? nullptr : castHelper( typeId, pointer );
	}


	virtual TypeId getType() const
	{
		return TypeId::getType< T >();
	}


	const IClassDefinition * getDefinition() const override
	{
		auto pointer = getPointer();
		return pointer == nullptr ? nullptr : getDefinition( pointer );
	}


	void throwBase() const override
	{
		auto pointer = getPointer();

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

		ObjectHandle baseProvider = definition->getBaseProvider( pointer );
		baseProvider.getStorage()->throwBase();
	}


private:
	const IClassDefinition * getDefinition(
		const ReflectedPolyStruct * polyStruct ) const 
	{
		return &polyStruct->getDefinition();
	}


	const IClassDefinition * getDefinition( const void * ) const 
	{
		return ObjectHandleStorageBase::getDefinition();
	}


	void * castHelper(
		const TypeId & typeId, const ReflectedPolyStruct * pStruct ) const
	{
		auto & definition = pStruct->getDefinition();
		ObjectHandle baseProvider = definition.getBaseProvider( getPointer() );
		return baseProvider.getStorage()->castHelper( typeId );
	}


	void * castHelper( const TypeId & typeId, ... ) const
	{
		T * pointer = getPointer();

		if (pointer == nullptr)
		{
			return nullptr;
		}

		ObjectHandle provider( *pointer );
		return provider.getStorage()->castHelper( typeId );
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
		std::unique_ptr< T > & pointer,
		const IClassDefinition * definition = nullptr )
		: ObjectHandleStoragePtr( pointer.get(), definition )
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


//==============================================================================
template< typename T >
class ObjectHandleT
{
public:
	//--------------------------------------------------------------------------
	ObjectHandleT()
		: storage_( nullptr )
	{
	}


	//--------------------------------------------------------------------------
	template< typename T2 >
	ObjectHandleT( const ObjectHandleT< T2 > & other )
		: storage_( other.storage_ )
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT( const ObjectHandle & other )
		: storage_( other.storage_ )
	{
	}


	//--------------------------------------------------------------------------
	ObjectHandleT( const nullptr_t & )
		: storage_( nullptr )
	{
	}


	struct NoValue {};

	template <typename T1, typename T2 = NoValue >
	struct all
		: std::conditional< T1::value, all< T2 >, std::false_type >::type
	{
	};


	template<>
	struct all< NoValue >
		: std::true_type
	{
	};

	template< typename T1 >
	struct not
		: std::false_type
	{
	};


	template<>
	struct not< std::false_type >
		: std::true_type
	{
	};


	//--------------------------------------------------------------------------
	//Allow conversions between const and non-const types
	template< typename ConvertType >
	ObjectHandleT(
		const ObjectHandleT< ConvertType > & handle,
		typename std::enable_if<
			all< std::is_base_of< ConvertType, T >,
				 not<
					all<
						std::is_const< T >,
						not< std::is_const< ConvertType > >
					>
				>
			>::value >::type * = nullptr  )
		: storage_( handle->storage_ )
	{
	}

	//--------------------------------------------------------------------------
	T * get() const
	{
		return reinterpret_cast< const ObjectHandle * >( this )->getBase< T >();
	}


	//--------------------------------------------------------------------------
	bool getId( RefObjectId & o_Id ) const 
	{
		return storage_->getId( o_Id );
	}


	//--------------------------------------------------------------------------
	const IClassDefinition * getDefinition() const
	{
		return storage_->getDefinition();
	}


	//--------------------------------------------------------------------------
	T & operator*() const
	{
		auto pObject = get();
		assert( pObject != nullptr );
		return *pObject;
	}


	//--------------------------------------------------------------------------
	T * operator->() const
	{
		return get();
	}


	//--------------------------------------------------------------------------
	bool operator ==( const ObjectHandle & other ) const
	{
		if (storage_ == other.storage_)
		{
			return true;
		}

		if (storage_ == nullptr || other.storage_ == nullptr)
		{
			return false;
		}

		return storage_->getRaw() == other.storage_->getRaw();
	}



	//--------------------------------------------------------------------------
	bool operator==( const void * p ) const
	{
		return get() == p;
	}


	//--------------------------------------------------------------------------
	bool operator!=( const void * p ) const
	{
		return !operator==( p );
	}


	//--------------------------------------------------------------------------
	ObjectHandleT & operator=( const nullptr_t & )
	{
		storage_ = nullptr;
		return *this;
	}


	//--------------------------------------------------------------------------
	ObjectHandleT & operator=( const ObjectHandle & other )
	{
		storage_ = other.storage_;
		return *this;
	}



	//--------------------------------------------------------------------------
	template< typename T2 >
	ObjectHandleT & operator=( const ObjectHandleT< T2 > & other )
	{
		storage_ = other.storage_;
		return *this;
	}


	//--------------------------------------------------------------------------
	bool operator<( const ObjectHandle & other ) const
	{
		if (storage_ == other.storage_)
		{
			return false;
		}

		if (storage_ == nullptr)
		{
			return true;
		}

		if (other.storage_ == nullptr)
		{
			return false;
		}

		return storage_->getRaw() < other.storage_->getRaw();
	}

private:
	friend ObjectHandle;
	template< typename T2 >
	friend class ObjectHandleT;

	std::shared_ptr< IObjectHandleStorage > storage_;
};


//==============================================================================
class ObjectHandle
{
public:
	static ObjectHandle getHandle( ReflectedPolyStruct & value );

	ObjectHandle();
	ObjectHandle( const std::shared_ptr< IObjectHandleStorage > & storage );
	ObjectHandle( const ObjectHandle & other );
	ObjectHandle( ObjectHandle && other );
	ObjectHandle( const nullptr_t & );

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle( const ObjectHandleT< T > & other )
		: storage_( other.storage_ )
	{
	}

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle(
		const T & value,
		const IClassDefinition * definition = nullptr )
		: storage_(
			new ObjectHandleStorage< T >(
				const_cast< T & >( value ), definition ) )
	{
	}


	//--------------------------------------------------------------------------
	template< typename T >
	static ObjectHandle makeStorageBackedProvider(
		const T & value,
		const IClassDefinition * definition = nullptr )
	{
		ObjectHandle provider;
		provider.storage_.reset(
			new ObjectHandleStorageCopy< T >(
			const_cast< T & >( value ), definition ) );
		return provider;
	}

	//--------------------------------------------------------------------------
	template< class T >
	T * getBase() const
	{
		if(storage_ == nullptr)
		{
			return nullptr;
		}
		static const TypeId s_Type = TypeId::getType< T >();
		auto srcDefinition = storage_->getDefinition();
		char * pRaw = static_cast< char * >( storage_->getRaw() );
		IClassDefinitionDetails::CastHelperCache * helperCache = nullptr;
		if (srcDefinition)
		{
			helperCache = srcDefinition->getDetails().getCastHelperCache();
			if (helperCache)
			{
				if (pRaw == nullptr)
				{
					return static_cast< T *>( nullptr );
				}
				auto helperFindIt = helperCache->find( s_Type );
				if (helperFindIt != helperCache->end())
				{
					if (!helperFindIt->second.first)
					{
						return nullptr;
					}
					pRaw += helperFindIt->second.second;
					return static_cast< T * >( reinterpret_cast< void * >( pRaw ) );
				}
			}
		}
		try
		{
			auto result = static_cast< T * >( storage_->castHelper( s_Type ) );
			return result;
		}
		catch ( const T * ptr )
		{
			if(helperCache)
			{
				helperCache->insert(
					std::make_pair(
						s_Type,
						std::make_pair( true,
							reinterpret_cast< const char * >( ptr ) - pRaw ) ) );
			}
			return const_cast< T * >( static_cast< const T * >( ptr ) );
		}
		catch ( ... )
		{
			if(helperCache)
			{
				helperCache->insert(
					std::make_pair(
						s_Type,
						std::make_pair( false, 0 ) ) );
			}
			return nullptr;
		}
	}

	void throwBase() const;
	bool getId( RefObjectId & o_Id ) const;
	const std::shared_ptr< IObjectHandleStorage > & getStorage() const;
	const IClassDefinition * getDefinition() const;
	bool isValid() const;
	bool operator ==( const ObjectHandle & other ) const;
	bool operator !=( const ObjectHandle & other ) const;
	ObjectHandle & operator=( const nullptr_t & );
	ObjectHandle & operator=( const ObjectHandle & other );
	ObjectHandle & operator=( ObjectHandle && other );
	bool operator<( const ObjectHandle & other ) const;

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle & operator=( const T & value )
	{
		storage_ .reset( new ObjectHandleStorage< T >( const_cast< T & >( value ) ) );
		return *this;
	}


private:
	template< typename T >
	friend class ObjectHandleT;

	std::shared_ptr< IObjectHandleStorage > storage_;
};


namespace variant
{
	template<typename T>
	ObjectHandle upcast( const ObjectHandleT< T > & v )
	{
		return ObjectHandle( v );
	}
}

#endif //OBJECT_HANDLE_HPP
