#include "pch.hpp"
#include "converters.hpp"


namespace PythonType
{


Converters::Converters( const BasicTypeConverters & basicTypeConverters,
	const ParentTypeConverters & parentTypeConverters )
	: basicTypeConverters_( basicTypeConverters )
	, parentTypeConverters_( parentTypeConverters )
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
	return parentTypeConverters_.toScriptType( inVariant, outObject );
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
	return parentTypeConverters_.toVariantWithParent( inObject,
		outVariant,
		parentHandle,
		childPath );
}


} // namespace PythonType

