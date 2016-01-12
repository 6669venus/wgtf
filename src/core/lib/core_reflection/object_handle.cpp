#include "object_handle.hpp"
#include "reflected_object.hpp"
#include "i_definition_manager.hpp"
#include "i_object_manager.hpp"
#include "object_handle_variant_storage.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/binary_stream.hpp"
#include "core_variant/variant.hpp"

//==============================================================================
// ObjectHandle
//==============================================================================

//------------------------------------------------------------------------------
/*static */ObjectHandle ObjectHandle::getHandle( ReflectedPolyStruct & value )
{
	auto defManager = value.getDefinition().getDefinitionManager();
	return defManager->getObjectManager()->getObject( &value );
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle()
	: storage_( nullptr )
{
}


//--------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const ObjectHandle & other )
	: storage_( other.storage_ )
{
}


ObjectHandle::ObjectHandle( ObjectHandle && other )
	: storage_( std::move( other.storage_ ) )
{
}


//--------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const std::shared_ptr< IObjectHandleStorage > & storage )
	: storage_( storage )
{
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const std::nullptr_t & )
	: storage_( nullptr )
{
}


ObjectHandle::ObjectHandle( const Variant & variant, const IClassDefinition * definition )
{
	if( auto handlePtr = variant.castPtr< ObjectHandle >() )
	{
		// avoid pointless nesting
		storage_ = handlePtr->storage_;
	}
	else
	{
		storage_ = std::make_shared< ObjectHandleVariantStorage >( variant, definition );
	}
}


ObjectHandle::ObjectHandle( Variant * variant, const IClassDefinition * definition )
{
	if( !variant )
	{
		// leave storage_ empty
		return;
	}

	if( auto handlePtr = variant->castPtr< ObjectHandle >() )
	{
		// avoid pointless nesting
		storage_ = handlePtr->storage_;
	}
	else
	{
		storage_ = std::make_shared< ObjectHandleVariantStorage >( variant, definition );
	}
}


//------------------------------------------------------------------------------
void * ObjectHandle::data() const
{
	return storage_ != nullptr ? storage_->data() : nullptr;
}


//------------------------------------------------------------------------------
TypeId ObjectHandle::type() const
{
	return storage_ != nullptr ? storage_->type() : nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::isValid() const
{
	return data() != nullptr;
}


//------------------------------------------------------------------------------
std::shared_ptr< IObjectHandleStorage > ObjectHandle::storage() const
{
	return storage_;
}


//------------------------------------------------------------------------------
const IClassDefinition * ObjectHandle::getDefinition( const IDefinitionManager & definitionManager ) const
{
	// Check if the type is a generic type
	// Generic types will provide different definitions for each instance
	auto definitionProvider = reflectedCast< const DefinitionProvider >( this->data(), this->type(), definitionManager );
	if (definitionProvider != nullptr)
	{
		return &definitionProvider->getDefinition();
	}

	const IClassDefinition * definition = definitionManager.getObjectDefinition( *this );
	if (definition != nullptr)
	{
		return definition;
	}

	return nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::getId( RefObjectId & o_Id ) const 
{
	if (storage_ == nullptr)
	{
		return false;
	}

	return storage_->getId( o_Id );
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator ==( const ObjectHandle & other ) const
{
	if (storage_ == other.storage_)
	{
		return true;
	}

	if (storage_ == nullptr || other.storage_ == nullptr)
	{
		return false;
	}

	auto left = storage_->data();
	auto right = other.storage_->data();
	if (left == right)
	{
		if (storage_->type() == other.storage_->type())
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator !=( const ObjectHandle & other ) const
{
	return !operator==( other );
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const std::nullptr_t & )
{
	storage_ = nullptr;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const ObjectHandle & other )
{
	storage_ = other.storage_;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( ObjectHandle && other )
{
	storage_ = std::move( other.storage_ );
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const std::shared_ptr< IObjectHandleStorage > & storage )
{
	storage_ = storage;
	return *this;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator<( const ObjectHandle & other ) const
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

	auto left = storage_->data();
	auto right = other.storage_->data();
	if (left == right)
	{
		return storage_->type() < other.storage_->type();
	}
	return left < right;
}


//------------------------------------------------------------------------------
void * reflectedCast( void * source, const TypeId & typeIdSource, const TypeId & typeIdDest, const IDefinitionManager & definitionManager )
{
	char * pRaw = static_cast< char * >( source );
	if (pRaw == nullptr)
	{
		return pRaw;
	}

	if (typeIdSource == typeIdDest)
	{
		return pRaw;
	}

	auto srcDefinition = definitionManager.getDefinition( typeIdSource.getName() );
	if (srcDefinition != nullptr)
	{
		auto helperCache = srcDefinition->getDetails().getCastHelperCache();
		auto helperFindIt = helperCache->find( typeIdDest );
		if (helperFindIt != helperCache->end())
		{
			if (!helperFindIt->second.first)
			{
				return nullptr;
			}
			return pRaw + helperFindIt->second.second;
		}

		auto dstDefinition = definitionManager.getDefinition( typeIdDest.getName() );
		if (dstDefinition != nullptr &&
			srcDefinition->canBeCastTo( *dstDefinition ))
		{
			auto result = srcDefinition->castTo( *dstDefinition, pRaw);
			assert( result != nullptr );
			helperCache->insert(
				std::make_pair(
				typeIdDest,
				std::make_pair( true,
				reinterpret_cast< const char * >( result ) - pRaw ) ) );
			return result;
		}
		else
		{
			helperCache->insert(
				std::make_pair(
				typeIdDest,
				std::make_pair( false,
				0 ) ) );
			return nullptr;
		}
	}

	return nullptr;
}


//------------------------------------------------------------------------------
ObjectHandle reflectedRoot( const ObjectHandle & source, const IDefinitionManager & definitionManager )
{
	if (!source.isValid())
	{
		return source;
	}

	auto root = source.storage();
	auto reflectedRoot = 
		definitionManager.getObjectDefinition( root ) != nullptr ? root : nullptr;
	for (;;)
	{
		auto inner = root->inner();
		if (inner == nullptr)
		{
			break;
		}
		root = inner;
		reflectedRoot = 
			definitionManager.getObjectDefinition( root ) != nullptr ? root : nullptr;
	}
	return ObjectHandle( reflectedRoot );
}

namespace
{

	template< typename Fn >
	void metaAction( const ObjectHandle& value, Fn fn )
	{
		const MetaType* metaType = nullptr;
		void* raw = nullptr;

		if( IObjectHandleStorage* storage = value.storage().get() )
		{
			metaType = Variant::findType( storage->type() );
			raw = storage->data();
		}

		fn( metaType, raw );
	}

}


//------------------------------------------------------------------------------
TextStream& operator<<( TextStream& stream, const ObjectHandle& value )
{
	metaAction( value, [&]( const MetaType* metaType, void* raw )
	{
		if( metaType && raw )
		{
			metaType->streamOut( stream, raw );
		}
		else
		{
			stream.setState( std::ios_base::failbit );
		}
	});

	return stream;
}


//------------------------------------------------------------------------------
TextStream& operator>>( TextStream& stream, ObjectHandle& value )
{
	metaAction( value, [&]( const MetaType* metaType, void* raw )
	{
		if( metaType && raw )
		{
			metaType->streamIn( stream, raw );
		}
		else
		{
			stream.setState( std::ios_base::failbit );
		}
	});

	return stream;
}


//------------------------------------------------------------------------------
BinaryStream& operator<<( BinaryStream& stream, const ObjectHandle& value )
{
	metaAction( value, [&]( const MetaType* metaType, void* raw )
	{
		if( metaType && raw )
		{
			metaType->streamOut( stream, raw );
		}
		else
		{
			stream.setState( std::ios_base::failbit );
		}
	});

	return stream;
}


//------------------------------------------------------------------------------
BinaryStream& operator>>( BinaryStream& stream, ObjectHandle& value )
{
	metaAction( value, [&]( const MetaType* metaType, void* raw )
	{
		if( metaType && raw )
		{
			metaType->streamIn( stream, raw );
		}
		else
		{
			stream.setState( std::ios_base::failbit );
		}
	});

	return stream;
}


