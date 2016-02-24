#include "pch.hpp"
#include "converter_queue.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


TypeId scriptTypeToTypeId( const PyScript::ScriptObject & scriptObject )
{
	const auto typeName = scriptObject.typeNameOfObject();

	// Convert Python type names to C++ type names
	// @see https://docs.python.org/2/library/types.html
	// Make sure that the corresponding type converter has been added to
	// ConverterQueue::init()
	if (strcmp( typeName, "NoneType" ) == 0)
	{
		return TypeId::getType< void * >();
	}
	else if (strcmp( typeName, "type" ) == 0)
	{
		return TypeId::getType< ObjectHandle >();
	}
	else if (strcmp( typeName, "bool" ) == 0)
	{
		return TypeId::getType< bool >();
	}
	else if (strcmp( typeName, "int" ) == 0)
	{
		return TypeId::getType< int >();
	}
	else if (strcmp( typeName, "long" ) == 0)
	{
		return TypeId::getType< digit >();
	}
	else if (strcmp( typeName, "float" ) == 0)
	{
		return TypeId::getType< double >();
	}
	else if (strcmp( typeName, "str" ) == 0)
	{
		return TypeId::getType< std::string >();
	}
	else if (strcmp( typeName, "unicode" ) == 0)
	{
		return TypeId::getType< std::wstring >();
	}
	else if (strcmp( typeName, "classobj" ) == 0)
	{
		return TypeId::getType< ObjectHandle >();
	}
	else if (strcmp( typeName, "instance" ) == 0)
	{
		return TypeId::getType< ObjectHandle >();
	}
	else if (strcmp( typeName, "instancemethod" ) == 0)
	{
		return TypeId::getType< ObjectHandle >();
	}
	else if (strcmp( typeName, "module" ) == 0)
	{
		return TypeId::getType< ObjectHandle >();
	}
	else if (strcmp( typeName, "tuple" ) == 0)
	{
		return TypeId::getType< Collection >();
	}
	else if (strcmp( typeName, "list" ) == 0)
	{
		return TypeId::getType< Collection >();
	}
	else if (strcmp( typeName, "dict" ) == 0)
	{
		return TypeId::getType< Collection >();
	}

	// Default type converter
	// New-style class names or other types not converted
	return TypeId::getType< ObjectHandle >();
}


ConverterQueue::ConverterQueue( IComponentContext & context )
	: context_( context )
	, listTypeConverter_( typeConverters_ )
	, tupleTypeConverter_( typeConverters_ )
	, dictTypeConverter_( typeConverters_ )
	, typeTypeConverter_( context )
	, pTypeConvertersInterface_( nullptr )
	, pDefaultTypeConverterInterface_( nullptr )
{
}


void ConverterQueue::init()
{
	// Register type converters for converting between PyObjects and Variant
	// Order is important
	// Last registered will be first searched
	// Make sure that the corresponding type converter has been added to
	// scriptTypeToTypeId()
	typeConverters_.registerTypeConverter( strTypeConverter_ );
	typeConverters_.registerTypeConverter( unicodeTypeConverter_ );
	typeConverters_.registerTypeConverter( listTypeConverter_ );
	typeConverters_.registerTypeConverter( tupleTypeConverter_ );
	typeConverters_.registerTypeConverter( dictTypeConverter_ );
	typeConverters_.registerTypeConverter( intTypeConverter_ );
	typeConverters_.registerTypeConverter( longTypeConverter_ );
	typeConverters_.registerTypeConverter( floatTypeConverter_ );
	const bool transferOwnership = false;
	pTypeConvertersInterface_ = context_.registerInterface(
		&typeConverters_,
		transferOwnership,
		IComponentContext::Reg_Local );
	pDefaultTypeConverterInterface_ = context_.registerInterface(
		&typeTypeConverter_,
		transferOwnership,
		IComponentContext::Reg_Local );
}


void ConverterQueue::fini()
{
	context_.deregisterInterface( pDefaultTypeConverterInterface_ );
	context_.deregisterInterface( pTypeConvertersInterface_ );

	// Deregister type converters for converting between PyObjects and Variant
	typeConverters_.deregisterTypeConverter( floatTypeConverter_ );
	typeConverters_.deregisterTypeConverter( longTypeConverter_ );
	typeConverters_.deregisterTypeConverter( intTypeConverter_ );
	typeConverters_.deregisterTypeConverter( dictTypeConverter_ );
	typeConverters_.deregisterTypeConverter( tupleTypeConverter_ );
	typeConverters_.deregisterTypeConverter( listTypeConverter_ );
	typeConverters_.deregisterTypeConverter( unicodeTypeConverter_ );
	typeConverters_.deregisterTypeConverter( strTypeConverter_ );
}


} // namespace PythonType

