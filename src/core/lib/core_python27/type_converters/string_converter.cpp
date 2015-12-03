#include "pch.hpp"

#include "string_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


bool StringConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	std::string value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	// Variant will create storage so don't worry about str going out of scope
	outVariant = Variant( value );
	return true;
}


bool StringConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< std::string >::storage_type >())
	{
		return false;
	}

	const std::string str = inVariant.value< std::string >();
	outObject = PyScript::ScriptString::create( str );
	return true;
}


} // namespace PythonType
