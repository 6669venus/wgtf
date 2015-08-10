#ifndef GENERIC_COMPONENT_PROVIDER_HPP
#define GENERIC_COMPONENT_PROVIDER_HPP

#include "simple_component_provider.hpp"

template< typename T >
class GenericComponentProvider : public SimpleComponentProvider
{
public:
	GenericComponentProvider( const char * component )
		: SimpleComponentProvider( component )
	{
	}

	GenericComponentProvider( const char * component,
		const size_t roles[] )
		: SimpleComponentProvider( component, roles )
	{
	}

	const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const override
	{
		if (typeId != TypeId::getType<T>())
		{
			return nullptr;
		}

		return SimpleComponentProvider::componentId( typeId, predicate );
	}
};

#endif