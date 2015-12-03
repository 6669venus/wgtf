#include "pch.hpp"

#include "unicode_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


bool UnicodeConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	std::wstring value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	// Variant will create storage so don't worry about str going out of scope
	outVariant = Variant( value );
	return true;
}


bool UnicodeConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< std::wstring >::storage_type >())
	{
		return false;
	}

	const std::wstring str = inVariant.value< std::wstring >();
	outObject = PyScript::ScriptUnicode::create( str );
	return true;
}


} // namespace PythonType
