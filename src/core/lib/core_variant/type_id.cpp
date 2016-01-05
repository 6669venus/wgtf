#include "type_id.hpp"
#include "core_reflection/object_handle.hpp"
#include "wg_types/hash_utilities.hpp"

#include <assert.h>
#include <string.h>

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

	auto name = getName();
	auto nameLen = strlen( name );

	auto voidType = TypeId::getType< void >().getName();
	auto voidTypeLen = strlen( voidType );

	// Test for T*
	auto voidPtrType = TypeId::getType< void * >().getName();
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
	auto voidObjectHandleType = TypeId::getType< ObjectHandleT< void > >().getName();
	auto voidObjectHandleTypeLen = strlen( voidObjectHandleType );
	assert( voidObjectHandleTypeLen > voidTypeLen );
	auto str = strstr( voidObjectHandleType, voidType );
	assert( str != nullptr );
	auto voidObjectHandleTypeLenPre = str - voidObjectHandleType;
	auto voidObjectHandleTypeLenPost = voidObjectHandleTypeLen - ( voidObjectHandleTypeLenPre + voidTypeLen );
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