#ifndef I_COMPONENT_PROVIDER_HPP
#define I_COMPONENT_PROVIDER_HPP

#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"

#include <vector>

class IComponentProvider
{
public:
	virtual ~IComponentProvider() {}

	virtual const char * component( 
		std::function< Variant ( size_t ) > & dataPredicate ) const { return nullptr; }

	// DEPRECATED //
	virtual const char * componentId( 
		const TypeId & typeId, std::function< bool ( size_t ) > & predicate ) const { return nullptr; }
};

#endif//I_COMPONENT_PROVIDER_HPP