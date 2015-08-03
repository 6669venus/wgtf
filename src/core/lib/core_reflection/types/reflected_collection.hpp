#ifndef REFLECTED_COLLECTION_IMPL_HPP
#define REFLECTED_COLLECTION_IMPL_HPP

#include "../interfaces/i_enum_generator.hpp"
#include "../interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"

//TODO Move this into a CPP instead
// ==============================================================================
class ReflectedCollectionImpl
	: public IEnumGenerator
{
public:
	ReflectedCollectionImpl( IBaseProperty * baseProperty )
		: baseProperty_( baseProperty )
	{
	}

	//==========================================================================
	~ReflectedCollectionImpl()
	{
		delete baseProperty_;
	}

	//==========================================================================
	Collection getCollection( const ObjectHandle & provider ) override
	{
		Variant value = baseProperty_->get( provider );
		Collection collection;
		bool ok = value.tryCast( collection );
		assert( ok );
		return collection;
	}

private:
	IBaseProperty * baseProperty_;
};

#endif // REFLECTED_COLLECTION_IMPL_HPP


