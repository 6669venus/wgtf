#include "pch.hpp"
#include "default_type_converter.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "core_python27/defined_instance.hpp"

#include <frameobject.h>


namespace PythonType
{
DefaultTypeConverter::DefaultTypeConverter( IComponentContext & context )
	: context_( context )
{
}


bool DefaultTypeConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant,
	const ReflectedPython::DefinedInstance & parent,
	const std::string & childPath )
{
	// Check for types.NoneType
	if (inObject.isNone())
	{
		void * noneType = nullptr;
		outVariant = Variant( noneType );
		return true;
	}

	// Default for anything that didn't match another type converter
	outVariant = ReflectedPython::DefinedInstance::findOrCreate( context_,
		inObject,
		parent,
		childPath );
	return true;
}


bool DefaultTypeConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject )
{
	// null void * -> None
	if (inVariant.typeIs< Variant::traits< void * >::storage_type >())
	{
		void * ptr = nullptr;
		const bool success = inVariant.tryCast< void * >( ptr );
		if (success && (ptr == nullptr))
		{
			outObject = PyScript::ScriptObject::none();
			return true;
		}
	}

	if (inVariant.typeIs<Variant::traits<ObjectHandle>::storage_type>())
	{
		ObjectHandle handle = inVariant.value<ObjectHandle>();
		auto contents = handle.getBase<ReflectedPython::DefinedInstance>();

		if (contents == nullptr)
		{
			return false;
		}

		outObject = contents->pythonObject();
		return true;
	}

	return false;
}
} // namespace PythonType
