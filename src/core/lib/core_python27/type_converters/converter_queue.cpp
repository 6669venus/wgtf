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
	, defaultTypeConverter_( context )
	, allConverters_( basicTypeConverters_, defaultTypeConverter_ )
	, listTypeConverter_( allConverters_ )
	, tupleTypeConverter_( allConverters_ )
	, dictTypeConverter_( allConverters_ )
	, pTypeConvertersInterface_( nullptr )
{
}


void ConverterQueue::init()
{
	// Register type converters for converting between PyObjects and Variant
	// Order is important
	// Last registered will be first searched
	// Make sure that the corresponding type converter has been added to
	// scriptTypeToTypeId()
	basicTypeConverters_.registerTypeConverter( strTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( unicodeTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( listTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( tupleTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( dictTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( intTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( longTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( floatTypeConverter_ );
	basicTypeConverters_.registerTypeConverter( noneTypeConverter_ );
	const bool transferOwnership = false;
	pTypeConvertersInterface_ = context_.registerInterface(
		&allConverters_,
		transferOwnership,
		IComponentContext::Reg_Local );
}


void ConverterQueue::fini()
{
	context_.deregisterInterface( pTypeConvertersInterface_ );
	// Type converters deregistered on destruction
}


} // namespace PythonType

