#include "pch.hpp"
#include "converters.hpp"
#include "default_converter.hpp"
#include "dict_converter.hpp"
#include "list_converter.hpp"
#include "tuple_converter.hpp"


namespace PythonType
{


Converters::Converters( const BasicTypeConverters & basicTypeConverters,
	DefaultConverter & defaultConverter,
	DictConverter & dictConverter,
	ListConverter & listConverter,
	TupleConverter & tupleConverter )
	: basicTypeConverters_( basicTypeConverters )
	, defaultConverter_( defaultConverter )
	, dictConverter_( dictConverter )
	, listConverter_( listConverter )
	, tupleConverter_( tupleConverter )
{
}


bool Converters::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) const
{
	const bool success = basicTypeConverters_.toScriptType( inVariant, outObject );
	if (success)
	{
		return true;
	}
	if (dictConverter_.toScriptType( inVariant, outObject ))
	{
		return true;
	}
	if (listConverter_.toScriptType( inVariant, outObject ))
	{
		return true;
	}
	if (tupleConverter_.toScriptType( inVariant, outObject ))
	{
		return true;
	}
	return defaultConverter_.toScriptType( inVariant, outObject );
}


bool Converters::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant,
	const ObjectHandle & parentHandle,
	const std::string & childPath ) const
{
	const bool success = basicTypeConverters_.toVariant( inObject, outVariant );
	if (success)
	{
		return true;
	}

	if (dictConverter_.toVariant( inObject,
		outVariant,
		parentHandle,
		childPath ))
	{
		return true;
	}
	if (listConverter_.toVariant( inObject,
		outVariant,
		parentHandle,
		childPath ))
	{
		return true;
	}
	if (tupleConverter_.toVariant( inObject,
		outVariant,
		parentHandle,
		childPath ))
	{
		return true;
	}
	return defaultConverter_.toVariant( inObject,
		outVariant,
		parentHandle,
		childPath );
}


} // namespace PythonType

