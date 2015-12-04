#include "pch.hpp"
#include "converter_queue.hpp"


namespace PythonType
{


ConverterQueue::ConverterQueue( IComponentContext & context )
	: context_( context )
	, listTypeConverter_( typeConverters_ )
	, tupleTypeConverter_( typeConverters_ )
	, dictTypeConverter_( typeConverters_ )
	, defaultTypeConverter_( context )
	, pTypeConvertersInterface_( nullptr )
{
}


void ConverterQueue::init()
{
	// Register type converters for converting between PyObjects and Variant
	// Order is important
	// Last registered will be first searched
	typeConverters_.registerTypeConverter( defaultTypeConverter_ );
	typeConverters_.registerTypeConverter( stringTypeConverter_ );
	typeConverters_.registerTypeConverter( unicodeTypeConverter_ );
	typeConverters_.registerTypeConverter( listTypeConverter_ );
	typeConverters_.registerTypeConverter( tupleTypeConverter_ );
	typeConverters_.registerTypeConverter( dictTypeConverter_ );
	typeConverters_.registerTypeConverter( intTypeConverter_ );
	typeConverters_.registerTypeConverter( boolTypeConverter_ );
	typeConverters_.registerTypeConverter( longTypeConverter_ );
	typeConverters_.registerTypeConverter( doubleTypeConverter_ );
	const bool transferOwnership = false;
	pTypeConvertersInterface_ = context_.registerInterface(
		&typeConverters_,
		transferOwnership,
		IComponentContext::Reg_Local );
}


void ConverterQueue::fini()
{
	// Deregister type converters for converting between PyObjects and Variant
	typeConverters_.deregisterTypeConverter( doubleTypeConverter_ );
	typeConverters_.deregisterTypeConverter( longTypeConverter_ );
	typeConverters_.deregisterTypeConverter( boolTypeConverter_ );
	typeConverters_.deregisterTypeConverter( intTypeConverter_ );
	typeConverters_.deregisterTypeConverter( tupleTypeConverter_ );
	typeConverters_.deregisterTypeConverter( dictTypeConverter_ );
	typeConverters_.deregisterTypeConverter( listTypeConverter_ );
	typeConverters_.deregisterTypeConverter( unicodeTypeConverter_ );
	typeConverters_.deregisterTypeConverter( stringTypeConverter_ );
	typeConverters_.deregisterTypeConverter( defaultTypeConverter_ );

	context_.deregisterInterface( pTypeConvertersInterface_ );
}


} // namespace PythonType

