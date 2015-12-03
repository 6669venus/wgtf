#include "pch.hpp"

#include "double_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


namespace PythonType
{


bool DoubleConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	double value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	outVariant = Variant( value );
	return true;
}


bool DoubleConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< double >::storage_type >())
	{
		return false;
	}
	double value;
	const auto isDouble = inVariant.tryCast< double >( value );
	if (!isDouble)
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
