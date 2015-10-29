#include "pch.hpp"

#include "type_converter.hpp"

#include "core_python27/python_meta_type.hpp"
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

	// Built-in types
	// https://docs.python.org/2/library/types.html
	if (type.name_ == "NoneType")
	{
		outObject = PyScript::ScriptObject::none();
		return true;
	}
	else if (type.name_ == "type")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyType_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "bool")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyBool_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "int")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyInt_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "long")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyLong_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "float")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFloat_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "complex")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyComplex_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "string")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyString_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "unicode")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyUnicode_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "tuple")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyTuple_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "list")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyList_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "dict")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyDict_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "dictionary")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyDict_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "function")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFunction_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "lambda")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFunction_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "generator")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFunction_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "code")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyCode_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "classobj")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyClass_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "instance")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyInstance_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "method")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyMethod_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "instancemethod")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyMethod_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "builtin_function_or_method")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyCFunction_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "module")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyModule_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "file")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFile_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "xrange")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyRange_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "slice")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PySlice_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "ellipsis")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyEllipsis_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "traceback")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyTraceBack_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "frame")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyFrame_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "buffer")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyBuffer_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "dictproxy")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyDictProxy_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "NotImplementedType")
	{
		assert( false && "Can't create not implemented type" );
		//outObject = PyScript::ScriptObject(
		//	reinterpret_cast< PyObject* >( &PyNotImplemented_Type ),
		//	PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return false;
	}
	else if (type.name_ == "getset_descriptor")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyGetSetDescr_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}
	else if (type.name_ == "member_descriptor")
	{
		outObject = PyScript::ScriptObject(
			reinterpret_cast< PyObject* >( &PyMemberDescr_Type ),
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
		return true;
	}

	// TODO NGT-1391 Can't re-create classes from a string
	assert( false && "PythonMetaType could not be converted" );
	outObject = PyScript::ScriptObject(
		reinterpret_cast< PyObject* >( &PyType_Type ),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	return true;
}


} // namespace PythonType
