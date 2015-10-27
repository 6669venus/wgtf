#include "pch.hpp"

#include "string_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


bool StringConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	// Get attribute as a string
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptString str = inObject.str( errorHandler );
	const char * value = str.c_str();

	// Let variant convert string to type
	// Variant will create storage so don't worry about str going out of scope
	outVariant = Variant( value );

	return true;
}


bool StringConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	const std::string str = inVariant.value< std::string >();

	outObject = PyScript::ScriptString::create( str );
	return true;
}


} // namespace PythonType
