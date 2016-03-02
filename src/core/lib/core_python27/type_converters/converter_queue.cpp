#include "pch.hpp"
#include "converter_queue.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


const TypeId & scriptTypeToTypeId( const PyScript::ScriptObject & scriptObject )
{
	// Convert Python type names to C++ type names
	// @see https://docs.python.org/2/library/types.html
	// Make sure that the corresponding type converter has been added to
	// ConverterQueue::init()
	if (scriptObject.isNone())
	{
		static auto type = TypeId::getType< void * >();
		return type;
	}
	else if (PyScript::ScriptType::check( scriptObject ))
	{
		static auto type = TypeId::getType< ObjectHandle >();
		return type;
	}
	else if (PyScript::ScriptBool::check( scriptObject ))
	{
		static auto type = TypeId::getType< bool >();
		return type;
	}
	else if (PyScript::ScriptInt::check( scriptObject ))
	{
		static auto type = TypeId::getType< int >();
		return type;
	}
	else if (PyScript::ScriptLong::check( scriptObject ))
	{
		static auto type = TypeId::getType< digit >();
		return type;
	}
	else if (PyScript::ScriptFloat::check( scriptObject ))
	{
		static auto type = TypeId::getType< double >();
		return type;
	}
	else if (PyScript::ScriptString::check( scriptObject ))
	{
		static auto type = TypeId::getType< std::string >();
		return type;
	}
	else if (PyScript::ScriptUnicode::check( scriptObject ))
	{
		static auto type = TypeId::getType< std::wstring >();
		return type;
	}
	else if (PyScript::ScriptClass::check( scriptObject ))
	{
		static auto type = TypeId::getType< ObjectHandle >();
		return type;
	}
	else if (PyScript::ScriptInstance::check( scriptObject ))
	{
		static auto type = TypeId::getType< ObjectHandle >();
		return type;
	}
	else if (PyScript::ScriptMethod::check( scriptObject ))
	{
		static auto type = TypeId::getType< ObjectHandle >();
		return type;
	}
	else if (PyScript::ScriptModule::check( scriptObject ))
	{
		static auto type = TypeId::getType< ObjectHandle >();
		return type;
	}
	else if (PyScript::ScriptTuple::check( scriptObject ))
	{
		static auto type = TypeId::getType< Collection >();
		return type;
	}
	else if (PyScript::ScriptList::check( scriptObject ))
	{
		static auto type = TypeId::getType< Collection >();
		return type;
	}
	else if (PyScript::ScriptDict::check( scriptObject ))
	{
		static auto type = TypeId::getType< Collection >();
		return type;
	}

	// Default type converter
	// New-style class names or other types not converted
	static auto type = TypeId::getType< ObjectHandle >();
	return type;
}


ConverterQueue::ConverterQueue( IComponentContext & context )
	: context_( context )
	, listTypeConverter_( typeConverters_ )
	, tupleTypeConverter_( typeConverters_ )
	, dictTypeConverter_( typeConverters_ )
	, typeTypeConverter_( context )
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
	typeConverters_.registerTypeConverter( typeTypeConverter_ );
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
}


void ConverterQueue::fini()
{
	// Deregister type converters for converting between PyObjects and Variant
	typeConverters_.deregisterTypeConverter( floatTypeConverter_ );
	typeConverters_.deregisterTypeConverter( longTypeConverter_ );
	typeConverters_.deregisterTypeConverter( intTypeConverter_ );
	typeConverters_.deregisterTypeConverter( tupleTypeConverter_ );
	typeConverters_.deregisterTypeConverter( dictTypeConverter_ );
	typeConverters_.deregisterTypeConverter( listTypeConverter_ );
	typeConverters_.deregisterTypeConverter( unicodeTypeConverter_ );
	typeConverters_.deregisterTypeConverter( strTypeConverter_ );
	typeConverters_.deregisterTypeConverter( typeTypeConverter_ );

	context_.deregisterInterface( pTypeConvertersInterface_ );
}


} // namespace PythonType

