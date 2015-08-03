#include "meta_base.hpp"
#include "meta_types.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/function_property.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"

BEGIN_EXPOSE( MetaBase, ReflectedPolyStruct, MetaNone() )
	EXPOSE( "nextMetaObject", nextAsHandle )
END_EXPOSE()

//==============================================================================
MetaBase::MetaBase()
	: nextMetaData_( NULL )
	, definitionName_( NULL )
{
}


//==============================================================================
/*virtual */MetaBase::~MetaBase()
{
	delete nextMetaData_;
}


ObjectHandle MetaBase::nextAsHandle() const
{
	if (nextMetaData_ == nullptr)
	{
		return nullptr;
	}

	return ObjectHandle( nextMetaData_, &nextMetaData_->getDefinition() );
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