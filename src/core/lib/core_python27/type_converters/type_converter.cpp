#include "pch.hpp"

#include "type_converter.hpp"

#include "core_python27/python_meta_type.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace PythonType
{


bool TypeConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	// Check for types.NoneType
	auto noneType = PyScript::ScriptType( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	if (noneType == inObject)
	{
		outVariant = Variant( PythonMetaType( inObject.typeNameOfObject() ) );
		return true;
	}

	// Check for types.TypeType
	if (PyScript::ScriptType::check( inObject ))
	{
		PyScript::ScriptType typeObject(
			reinterpret_cast< PyTypeObject * >( inObject.get() ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		outVariant = Variant( PythonMetaType( typeObject.name() ) );
		return true;
	}

	// Check for types.ClassType
	if (PyScript::ScriptClass::check( inObject ))
	{
		PyScript::ScriptErrorPrint errorHandler;
		PyScript::ScriptObject className =
			inObject.getAttribute( "__name__", errorHandler );

		outVariant = Variant( PythonMetaType(
			className.str( errorHandler ).c_str() ) );
		return true;
	}

	return false;
}


bool TypeConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	if (!inVariant.typeIs< Variant::traits< PythonMetaType >::storage_type >())
	{
		return false;
	}

	const auto type = inVariant.value< PythonMetaType >();

	outObject = PyScript::ScriptString::create( type.name_ );
	return true;
}


} // namespace PythonType
