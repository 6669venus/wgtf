#ifndef OBJECT_HANDLE_HPP
#define OBJECT_HANDLE_HPP

#define DEPRECATE_OBJECT_HANDLE 1
#if DEPRECATE_OBJECT_HANDLE
#define DEPRECATE_OBJECT_HANDLE_MSG( format, ... ) {				\
	char buffer[1024];												\
	sprintf_s( buffer, 1024, format, ## __VA_ARGS__ );				\
	wchar_t msg[1024];												\
	mbstowcs( msg, buffer, 1024 );									\
	_wassert( msg, _CRT_WIDE(__FILE__), __LINE__ );					\
}
#else
#define DEPRECATE_OBJECT_HANDLE_MSG( format, ... ) {				\
	NGT_TRACE_MSG( format, ## __VA_ARGS__ );						\
}
#endif

/*
An ObjectHandle contains a data type and its associated ClassDefintion.
ObjectHandles store reflection data at runtime and are flexible enough to store
most types, including references, pointers and structures.
Use an ObjectHandle when a model or property needs to be exposed to QML.
Also use when working with reflection containers or reflected objects.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "object_handle_storage.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/type_id.hpp"
#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition.hpp"
#include <cassert>
#include <memory>
#include <map>
#include <unordered_map>

template<typename T> class ObjectHandleT;
class ReflectedPolyStruct;

//==============================================================================
class ObjectHandle
{
public:
	static ObjectHandle getHandle( ReflectedPolyStruct & value );

	ObjectHandle();
	ObjectHandle( const ObjectHandle & other );
	ObjectHandle( ObjectHandle && other );
	ObjectHandle( const std::nullptr_t & );

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle( const ObjectHandleT< T > & other );

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
	ObjectHandle(
		std::unique_ptr<T> && value,
		const IClassDefinition * definition = nullptr)
		: storage_(
		new ObjectHandleStorage< std::unique_ptr< T > >(
		std::move(value), definition ) )
	{
	}

	//--------------------------------------------------------------------------
	template< class T >
	T * reflectedCast( const IDefinitionManager & definitionManager ) const
	{
		return reinterpret_cast< T * >( reflectedCast( TypeId::getType< T >(), definitionManager ) );
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
		if (s_Type != storage_->type())
		{
			try
			{
				storage_->throwBase();
			}
			catch (T* value)
			{
				DEPRECATE_OBJECT_HANDLE_MSG( "DEPRECATED OBJECTHANDLE: Type '%s' stored in ObjectHandle does not match type explicitly queried type '%s'\n", storage_->type().getName(), s_Type.getName() );
				return value;
			}
			catch(...)
			{
			}
			return nullptr;
		}
		return static_cast< T * >( storage_->data() );
	}

	void * data() const;
	TypeId type() const;
	bool isValid() const;

	const IClassDefinition * getDefinition( const IDefinitionManager & definitionManager ) const;
	bool getId( RefObjectId & o_Id ) const;
	void throwBase() const;
	bool operator ==( const ObjectHandle & other ) const;
	bool operator !=( const ObjectHandle & other ) const;
	ObjectHandle & operator=( const std::nullptr_t & );
	ObjectHandle & operator=( const ObjectHandle & other );
	ObjectHandle & operator=( ObjectHandle && other );
	bool operator<( const ObjectHandle & other ) const;

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle & operator=( const T & value )
	{
		static_assert(!std::is_copy_constructible<T>::value,
			"Type is not copy constructable, try using std::move(value)");
		storage_ .reset( new ObjectHandleStorage< T >( const_cast< T & >( value ) ) );
		return *this;
	}

	//--------------------------------------------------------------------------
	template< typename T >
	ObjectHandle & operator=( std::unique_ptr< T >&& value )
	{
		storage_.reset(new ObjectHandleStorage< std::unique_ptr< T > >(std::move(value)));
		return *this;
	}

	//--------------------------------------------------------------------------
	void * reflectedCast( const TypeId & typeId, const IDefinitionManager & definitionManager ) const;

private:
	template< typename T >
	friend class ObjectHandleT;

	std::shared_ptr< IObjectHandleStorage > storage_;
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
	ObjectHandleT( const std::nullptr_t & )
		: storage_( nullptr )
	{
	}


	//--------------------------------------------------------------------------
	ObjectHandleT( const ObjectHandleT & other )
		: storage_( other.storage_ )
		, getter_( other.getter_ )
	{
	}


	//--------------------------------------------------------------------------
	template< typename T2 >
	ObjectHandleT( const ObjectHandleT< T2 > & other )
	: storage_( other.storage_ )
	{
		getter_ = [=]( const ObjectHandle & handle ) -> T * { return static_cast< T * >( other.getter_( handle ) ); };
	}


	//--------------------------------------------------------------------------
	T * get() const
	{
		if (getter_ != nullptr)
		{
			return getter_( *this );
		}
		return nullptr;
	}


	//--------------------------------------------------------------------------
	const IClassDefinition * getDefinition( IDefinitionManager & definitionManager ) const
	{
		auto handle = ObjectHandle( *this );
		return handle.getDefinition( definitionManager );
	}


	//--------------------------------------------------------------------------
	bool getId( RefObjectId & o_Id ) const
	{
		return storage_ != nullptr ? storage_->getId( o_Id ) : false;
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
	bool operator==( const ObjectHandle & other ) const
	{
		return ObjectHandle( *this ) == other;
	}


	//--------------------------------------------------------------------------
	bool operator!=( const ObjectHandle & other ) const
	{
		return ObjectHandle( *this ) != other;
	}


	//--------------------------------------------------------------------------
	bool operator<( const ObjectHandle & other ) const
	{
		return ObjectHandle( *this ) < other;
	}


	//--------------------------------------------------------------------------
	static ObjectHandleT cast( const ObjectHandle & other )
	{
		assert( other.type() == TypeId::getType< T >() );
		return ObjectHandleT< T >( other );
	}

private:
	//--------------------------------------------------------------------------
	ObjectHandleT( const ObjectHandle & other )
	: storage_( other.storage_ )
	{
		getter_ = []( const ObjectHandle & handle ) -> T * { return handle.getBase< T >(); };
	}


	friend ObjectHandle;
	template< typename T2 >
	friend class ObjectHandleT;

	std::shared_ptr< IObjectHandleStorage > storage_;
	std::function< T*( const ObjectHandle & handle ) > getter_;
};

template< typename T >
ObjectHandle::ObjectHandle( const ObjectHandleT< T > & other )
: storage_( other.storage_ )
{
}

template<typename T>
ObjectHandle upcast( const ObjectHandleT< T > & v )
{
	return ObjectHandle( v );
}

template<typename T>
bool downcast( ObjectHandleT< T >* v, const ObjectHandle& storage)
{
	if(v && storage.type() == TypeId::getType< T >())
	{
		*v = ObjectHandleT< T >::cast( storage );
	}
	return true;
}

#endif //OBJECT_HANDLE_HPP
