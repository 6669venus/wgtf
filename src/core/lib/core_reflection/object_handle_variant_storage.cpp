#include "i_definition_manager.hpp"
#include "object_handle_variant_storage.hpp"


ObjectHandleVariantStorage::ObjectHandleVariantStorage( const Variant& variant, const IClassDefinition* definition ):
	variant_( variant ),
	variantPtr_( &variant_ ),
	definition_( definition )
{
}


ObjectHandleVariantStorage::ObjectHandleVariantStorage( Variant* variant, const IClassDefinition* definition ):
	variant_(),
	variantPtr_( variant ),
	definition_( definition )
{
}


void* ObjectHandleVariantStorage::data() const
{
	return variantPtr_->castPtr< void >();
}


TypeId ObjectHandleVariantStorage::type() const
{
	if( const std::type_info* pointedType = variantPtr_->type()->pointedType() )
	{
		return TypeId( pointedType->name() );
	}
	else
	{
		return variantPtr_->type()->typeId();
	}
}


bool ObjectHandleVariantStorage::getId( RefObjectId& o_Id ) const
{
	return false;
}


const IClassDefinition* ObjectHandleVariantStorage::getDefinition( const IDefinitionManager& definitionManager ) const
{
	return definition_;
}


void ObjectHandleVariantStorage::throwBase() const
{
	if( void* ptr = variantPtr_->castPtr< void >() )
	{
		variantPtr_->type()->throwPtr( ptr, !variantPtr_->isPointer() );
	}
}


