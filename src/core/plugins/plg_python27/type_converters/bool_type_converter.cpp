#include "pch.hpp"

#include "bool_type_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


bool BoolConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	// Get attribute as a string
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptString str = inObject.str( errorHandler );
	const char * value = str.c_str();

	const bool isTrue = (strcmp( value, "True" ) == 0);
	if (!isTrue)
	{
		const bool isFalse = (strcmp( value, "False" ) == 0);
		if (!isFalse)
		{
			// Is not a boolean
			return false;
		}
	}

	// Let variant convert string to type
	// Variant will create storage so don't worry about str going out of scope
	outVariant = Variant( isTrue );

	return true;
}


bool BoolConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	const bool value = inVariant.value< bool >();

	outObject = PyScript::ScriptString::create( value ? "True" : "False" );
	return true;
}


} // namespace PythonType
