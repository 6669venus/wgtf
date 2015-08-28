#ifndef META_UTILITIES_HPP
#define META_UTILITIES_HPP

#include "../utilities/reflection_utilities.hpp"
#include "../property_accessor.hpp"
#include "../interfaces/i_base_property.hpp"
#include "meta_base.hpp"

class IBaseProperty;

//==============================================================================
template< class T >
const T * findFirstMetaData( const MetaBase * metaData )
{
	while ( metaData != NULL )
	{
		const T * targetType =
			ReflectionUtilities::dynamicCast< const T >( *metaData );
		if (targetType != NULL)
		{
			return targetType;
		}
		metaData = metaData->next();
	}
	return NULL;
}


//==============================================================================
template< class T >
const T * findFirstMetaData( const PropertyAccessor & accessor )
{
	const MetaBase * metaData = accessor.getMetaData();
	const T * targetMetaData = findFirstMetaData< T >( metaData );
	if (targetMetaData != NULL)
	{
		return targetMetaData;
	}
	PropertyAccessor parentAccessor = accessor.getParent();
	if (parentAccessor.getValue().typeIs< Collection >() == false)
	{
		return NULL;
	}
	return findFirstMetaData< T >( parentAccessor );
}


//==============================================================================
template< class T >
const T * findFirstMetaData( IBaseProperty * pProperty )
{
	if (pProperty == NULL)
	{
		return NULL;
	}
	const MetaBase * metaData = pProperty->getMetaData();
	return findFirstMetaData< T >( metaData );
}


//==============================================================================
template< class T >
const T * findFirstMetaData( const IClassDefinition & definition )
{
	const MetaBase * metaData = definition.getMetaData();
	return findFirstMetaData< T >( metaData );
}

#endif //META_UTILITIES_HPP