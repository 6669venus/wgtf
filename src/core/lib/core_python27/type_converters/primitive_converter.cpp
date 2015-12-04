#include "pch.hpp"

#include "primitive_converter.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"

#include <longintrepr.h>

#include <string>


namespace PythonType
{


template< typename T >
bool PrimitiveConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	T value;
	const bool result = inObject.convertTo( value, PyScript::ScriptErrorClear() );
	if (!result)
	{
		return false;
	}

	outVariant = Variant( value );
	return true;
}


template< typename T >
bool PrimitiveConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< T >::storage_type >())
	{
		return false;
	}
	T value;
	const auto isPrimitive = inVariant.tryCast< T >( value );
	if (!isPrimitive)
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


// Explicit instantiations
template class PrimitiveConverter< bool >;
template class PrimitiveConverter< int >;
template class PrimitiveConverter< digit >;
template class PrimitiveConverter< double >;
template class PrimitiveConverter< std::string >;
template class PrimitiveConverter< std::wstring >;


} // namespace PythonType
