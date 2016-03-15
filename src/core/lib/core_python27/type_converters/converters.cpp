#include "pch.hpp"
#include "converters.hpp"


namespace PythonType
{


Converters::Converters( const BasicTypeConverters & basicTypeConverters,
	const DefaultConverter & defaultConverter )
	: basicTypeConverters_( basicTypeConverters )
	, defaultConverter_( defaultConverter )
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
	return defaultConverter_.toScriptType( inVariant, outObject );
}


bool Converters::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant,
	const PyScript::ScriptObject & parentObject,
	const std::string & childPath ) const
{
	const bool success = basicTypeConverters_.toVariant( inObject, outVariant );
	if (success)
	{
		return true;
	}
	return defaultConverter_.toVariant( inObject,
		outVariant,
		parentObject,
		childPath );
}


} // namespace PythonType

