#include "meta_base.hpp"
#include "meta_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

BEGIN_EXPOSE( MetaBase, MetaNone() )
END_EXPOSE()

//==============================================================================
MetaBase::MetaBase()
	: nextMetaData_( NULL )
{
}


//==============================================================================
/*virtual */MetaBase::~MetaBase()
{
	delete nextMetaData_;
}


//==============================================================================
void MetaBase::initialise( const TypeId & typeId, const ObjectHandleT< MetaBase > & handle )
{
	typeId_ = typeId;
	handle_ = handle;
}


//==============================================================================
const MetaBase & operator + ( const MetaBase & left, const MetaBase & right )
{
	// traverse to the end of the linked list
	const MetaBase * next = left.next();
	const MetaBase * last = &left;
	while( next != NULL )
	{
		last = next;
		next = next->next();
	};

	// hook into the end
	last->setNext( &right );

	return left; 
}