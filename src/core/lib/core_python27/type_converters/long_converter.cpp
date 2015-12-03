#include "pch.hpp"

#include "long_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"

#include <longintrepr.h>

namespace PythonType
{


bool LongConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	digit value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	outVariant = Variant( value );
	return true;
}


bool LongConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< digit >::storage_type >())
	{
		return false;
	}
	digit value;
	const auto isLong = inVariant.tryCast< digit >( value );
	if (!isLong)
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
