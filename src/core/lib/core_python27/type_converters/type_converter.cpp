#include "pch.hpp"

#include "type_converter.hpp"

#include "python_meta_type.hpp"

#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


#include <frameobject.h>


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
		void * noneType = nullptr;
		outVariant = Variant( noneType );
		return true;
	}

	// Check for types.TypeType
	if (PyScript::ScriptType::check( inObject ))
	{
		PyScript::ScriptType typeObject(
			reinterpret_cast< PyTypeObject * >( inObject.get() ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		outVariant = Variant( PythonMetaType( typeObject ) );
		return true;
	}

	// Check for types.ClassType
	if (PyScript::ScriptClass::check( inObject ))
	{
		PyScript::ScriptClass classType = PyScript::ScriptClass(
			inObject.get(),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		outVariant = Variant( PythonMetaType( classType ) );
		return true;
	}

	return false;
}


bool TypeConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject ) /* override */
{
	// null void * -> None
	if (inVariant.typeIs< Variant::traits< void * >::storage_type >())
	{
		void * ptr = nullptr;
		const bool success = inVariant.tryCast< void * >( ptr );
		if (success && (ptr == nullptr))
		{
			outObject = PyScript::ScriptObject( Py_None,
				PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
			return true;
		}
	}

	// PythonMetaType -> PyTypeObject
	if (!inVariant.typeIs< Variant::traits< PythonMetaType >::storage_type >())
	{
		return false;
	}

	const auto type = inVariant.value< PythonMetaType >();
	outObject = PyScript::ScriptObject(
		type.type().get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	return true;
}


} // namespace PythonType
