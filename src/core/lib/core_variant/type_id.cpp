#include "type_id.hpp"
#include "core_reflection/object_handle.hpp"
#include "wg_types/hash_utilities.hpp"

#include <assert.h>
#include <string.h>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif


std::string demangleTypeName( const char* name )
{
#ifdef __GNUG__
	int status = -1;
	char* demangledName = abi::__cxa_demangle( name, nullptr, nullptr, &status );
	std::unique_ptr<char, std::function<void(void*)>> res( demangledName, std::free );
	return status == 0 ? res.get() : name;
}
#else
	return name;
#endif
}


//==============================================================================
TypeId::TypeId( const std::string & name )
	: name_( nullptr )
	, hashCode_( 0 )
	, owns_( false )
{
	if (name.empty())
	{
		return;
	}

	auto len = name.size();
	auto buffer = new char[len + 1];
	strncpy( buffer, name.c_str(), len + 1 );

	name_ = buffer;
	hashCode_ = HashUtilities::compute( buffer );
	owns_ = true;
}


//==============================================================================
TypeId::TypeId( const char * name )
	: name_( name )
	, hashCode_( name != nullptr ? HashUtilities::compute( name ) : 0 )
	, owns_( false )
{
}


//==============================================================================
TypeId::TypeId( const char * name, uint64_t hashCode )
	: name_( name )
	, hashCode_( hashCode )
	, owns_( false )
{
}


//==============================================================================
TypeId::TypeId( const TypeId & other )
	: name_( other.name_ )
	, hashCode_( other.hashCode_ )
	, owns_( other.owns_ )
{
	if (!owns_)
	{
		return;
	}

	auto len = strlen( name_ );
	auto buffer = new char[len + 1];
	strncpy( buffer, name_, len + 1 );
	name_ = buffer;
}

//==============================================================================
TypeId::~TypeId()
{
	if (owns_)
	{
		delete[] name_;
	}
}


//==============================================================================
bool TypeId::isPointer() const
{
	return removePointer( nullptr );
}


//==============================================================================
TypeId TypeId::removePointer() const
{
	TypeId typeId = *this;
	removePointer( &typeId );
	return typeId;
}


//==============================================================================
bool TypeId::removePointer( TypeId * typeId ) const
{
	// Temporary hack - Using string manipulation to determine if a typeId points to a pointer type (or ObjectHandle type)
	// Necessary whilst we still allow TypeIds to be constructed by string.

	auto nameString = demangleTypeName( getName() );
	auto name = nameString.c_str();
	auto nameLen = strlen( name );

	auto voidTypeString = demangleTypeName( TypeId::getType< void >().getName() );
	auto voidType = voidTypeString.c_str();
	auto voidTypeLen = strlen( voidType );

	// Test for T*
	auto voidPtrTypeString = demangleTypeName( TypeId::getType< void * >().getName() );
	auto voidPtrType = voidPtrTypeString.c_str();
	auto voidPtrTypeLen = strlen( voidPtrType );
	assert( voidPtrTypeLen > voidTypeLen );
	assert( strncmp( voidType, voidPtrType, voidTypeLen ) == 0 );
	auto ptrType = voidPtrType + voidTypeLen;
	auto ptr = strstr( name, ptrType );
	if (ptr != nullptr)
	{
		if (typeId != nullptr)
		{
			auto type = std::string( name, ptr - name );
			*typeId = TypeId( type.c_str() );
		}
		return true;
	}

	// Test for ObjectHandle<T>
	auto voidObjectHandleTypeString = demangleTypeName( TypeId::getType< ObjectHandleT< void * > >().getName() );
	auto voidObjectHandleType = voidObjectHandleTypeString.c_str();
	auto voidObjectHandleTypeLen = strlen( voidObjectHandleType );
	assert( voidObjectHandleTypeLen > voidPtrTypeLen );
	auto str = strstr( voidObjectHandleType, voidPtrType );
	assert( str != nullptr );
	auto voidObjectHandleTypeLenPre = str - voidObjectHandleType;
	auto voidObjectHandleTypeLenPost = voidObjectHandleTypeLen - ( voidObjectHandleTypeLenPre + voidPtrTypeLen );
	assert( voidObjectHandleTypeLenPre > 0 );
	assert( voidObjectHandleTypeLenPost > 0 );
	if (strncmp( name, voidObjectHandleType, voidObjectHandleTypeLenPre ) == 0 &&
		strncmp( name + nameLen - voidObjectHandleTypeLenPost, voidObjectHandleType + voidObjectHandleTypeLen - voidObjectHandleTypeLenPost, voidObjectHandleTypeLenPost ) == 0)
	{
		if (typeId != nullptr)
		{
			auto type = std::string( name + voidObjectHandleTypeLenPre, nameLen - ( voidObjectHandleTypeLenPre + voidObjectHandleTypeLenPost ) );
			*typeId = TypeId( type );
		}
		return true;
	}

	return false;
}


//==============================================================================
TypeId & TypeId::operator = ( const TypeId & other )
{
	if (owns_)
	{
		delete[] name_;
	}

	name_ = other.name_;
	hashCode_ = other.hashCode_;
	owns_ = other.owns_;

	if (!owns_)
	{
		return *this;
	}

	auto len = strlen( name_ );
	auto buffer = new char[len + 1];
	strncpy( buffer, name_, len + 1 );
	name_ = buffer;
	
	return *this;
}


//==============================================================================
bool TypeId::operator == ( const TypeId & other ) const
{
	return hashCode_ == other.getHashcode();
}


//==============================================================================
bool TypeId::operator != ( const TypeId & other ) const
{
	return hashCode_ != other.getHashcode();
}


//==============================================================================
bool TypeId::operator < (const TypeId & other ) const
{
	return hashCode_ < other.getHashcode();
}