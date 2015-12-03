#include "pch.hpp"

#include "bool_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace PythonType
{


bool BoolConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	bool value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	outVariant = Variant( value );
	return true;
}


bool BoolConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< bool >::storage_type >())
	{
		return false;
	}
	bool value;
	const auto isBool = inVariant.tryCast< bool >( value );
	if (!isBool)
	{
		return false;
	}

	auto result = PyScript::ScriptObject::createFrom( value );
	if (!result.exists())
	{
		return false;
	}

	outObject = result;
	return true;
}


} // namespace PythonType
