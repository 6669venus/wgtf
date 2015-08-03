#ifndef I_COMPONENT_PROVIDER_HPP
#define I_COMPONENT_PROVIDER_HPP

#include "variant/type_id.hpp"

#include <vector>

class IComponentProvider
{
public:
	virtual ~IComponentProvider() {}

	virtual const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const = 0;
};

#endif//I_COMPONENT_PROVIDER_HPP