#include "pch.hpp"

#include <longintrepr.h>

#include "reflection_module.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "defined_instance.hpp"

#include <cassert>

namespace
{

/// State storage for static functions attached to Python
/// TODO Move to ReflectionModule somehow?
IDefinitionManager * g_definitionManager = nullptr;
IObjectManager * g_objectManager = nullptr;
const PythonTypeConverters * g_typeConverters = nullptr;


/**
 *	Create a reflected object and return it to Python.
 *	
 *	@param self the reflection module.
 *	@param args first argument must be a TypeId.
 *		e.g. reflection.create("TypeId")
 *	@param argument keywords
 *		e.g. reflection.create(objectType="TypeId")
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the TypeId does not have a valid definition.
 *	
 *	@return reflected object.
 */
static PyObject * py_create( PyObject * self, PyObject * args, PyObject * kw )
{
	char * objectType = nullptr;

	static char *keywords [] = {
		"objectType",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords( args, kw, "s", keywords, &objectType ))
	{
		// PyArg_ParseTupleAndKeywords sets error indicator
		return nullptr;
	}
	if (objectType[0] == '\0')
	{
		PyErr_Format( PyExc_TypeError,
			"Can not create a definition from an empty string." );
		return nullptr;
	}

	assert( g_definitionManager != nullptr );
	const IClassDefinition * pDefinition =
		g_definitionManager->getDefinition( objectType );
	if (pDefinition == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Could not find a definition for \"%s\"\n", objectType );
		return nullptr;
	}

	ObjectHandle object = pDefinition->create();
	assert( object != nullptr );

	// TODO NGT-1052

	// Return none to pass the test
	Py_RETURN_NONE;
}


/**
 *	
 *	
 *	@param instance the reflected Python object to test.
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
PyObject * parseArguments( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	PyObject * object = nullptr;

	static char *keywords [] = {
		"object",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords( args, kw, "O", keywords, &object ))
	{
		// PyArg_ParseTupleAndKeywords sets error indicator
		return nullptr;
	}
	if (object == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Cannot test null object." );
		return nullptr;
	}

	return object;
}


/**
 *	Test converting a Python object to a reflected object.
 *	
 *	@param instance the reflected Python object to test.
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * commonConversionTest(
	ReflectedPython::DefinedInstance& instance )
{
	// Check that the Python object's definition is working
	// At the moment a different definition is made for each Python object
	// instance
	{
		const IClassDefinition & genericDefinition = instance.getDefinition();

		const ClassDefinition * pGenericClassDefinition =
			dynamic_cast< const ClassDefinition * >( &genericDefinition );
		if (pGenericClassDefinition == nullptr)
		{
			PyErr_Format( PyExc_TypeError,
				"Failed to get correct definition." );
			return nullptr;
		}

		const IClassDefinitionDetails& details =
			pGenericClassDefinition->getDetails();

		const ReflectedPython::DefinitionDetails * pPythonDefinition =
			dynamic_cast< const ReflectedPython::DefinitionDetails * >( &details );
		if (pPythonDefinition == nullptr)
		{
			PyErr_Format( PyExc_TypeError,
				"Failed to get correct definition." );
			return nullptr;
		}
	}

	// Test getting properties from the instance
	// Using the Python object's definition

	// Convert Python "True" -> C++ "true"
	{
		// @see PyBoolObject, PyIntObject
		const bool boolTest = false;
		const bool setSuccess = instance.set< bool >(
			"boolTest", boolTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot set property." );
			return nullptr;
		}

		bool boolCheck = true;
		const bool getSuccess = instance.get< bool >( "boolTest", boolCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (boolTest != boolCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}
	{
		// @see PyIntObject
		const long intTest = 2;
		const bool setSuccess = instance.set< long >( "intTest", intTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}

		long intCheck = 1;
		const bool getSuccess = instance.get< long >( "intTest", intCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (intTest != intCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}
	{
		// @see PyLongObject
		const digit longTest = 2;
		const bool setSuccess = instance.set< digit >( "longTest", longTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}

		digit longCheck = 1;
		const bool getSuccess = instance.get< digit >( "longTest", longCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (longTest != longCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}
	{
		// @see PyFloatObject
		const double floatTest = 2.0;
		const bool setSuccess = instance.set< double >( "floatTest", floatTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}

		double floatCheck = 1.0;
		const bool getSuccess = instance.get< double >( "floatTest", floatCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		// TODO direct floating point comparison is bad
		if (floatTest != floatCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}
	// TODO structs
	//{
	//	// @see PyComplexObject
	//	Py_complex complexTest;
	//	complexTest.real = 1.0;
	//	complexTest.imag = 0.0;
	//	const bool setSuccess = instance.set< Py_complex >(
	//		"complexTest", complexTest );

	//	if (!setSuccess)
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Cannot get property." );
	//		return nullptr;

	//	Py_complex complexCheck;
	//	complexCheck.real = 0.0;
	//	complexCheck.imag = 1.0;
	//	const bool getSuccess = instance.get< Py_complex >(
	//		"complexTest", complexCheck );

	//	if (!getSuccess)
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Cannot get property." );
	//		return nullptr;
	//	}
	//	// TODO direct floating point comparison is bad
	//	if ((complexTest.real != complexCheck.real) ||
	//		(complexTest.imag != complexCheck.imag))
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Got invalid property." );
	//		return nullptr;
	//	}
	//}
	{
		// @see PyStringObject
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"stringTest", stringTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot set property." );
			return nullptr;
		}

		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"stringTest", stringCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (stringTest != stringCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}
	// TODO causes memory leak
	//{
	//	const std::wstring unicodeTest = L"String was set";
	//	const bool setSuccess = instance.set< std::wstring >(
	//		"unicodeTest", unicodeTest );
	//	if (!setSuccess)
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Cannot get property." );
	//		return nullptr;
	//	}

	//	std::wstring unicodeCheck = L"Fail";
	//	const bool getSuccess = instance.get< std::wstring >(
	//		"unicodeTest", unicodeCheck );

	//	if (!getSuccess)
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Cannot get property." );
	//		return nullptr;
	//	}
	//	if (unicodeTest != unicodeCheck)
	//	{
	//		PyErr_Format( PyExc_TypeError,
	//			"Got invalid property." );
	//		return nullptr;
	//	}
	//}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "methodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		if (returnValue != "Method test was run")
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot invoke property." );
			return nullptr;
		}
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "classMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		if (returnValue != "Class method test was run")
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot invoke property." );
			return nullptr;
		}
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "staticMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		if (returnValue != "Static method test was run")
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot invoke property." );
			return nullptr;
		}
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest1", parameters );

		const std::string returnValue = result.value< std::string >();
		if (returnValue != "Function test was run")
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot invoke property." );
			return nullptr;
		}
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest2", parameters );

		const std::string returnValue = result.value< std::string >();
		if (returnValue != "Callable class test was run")
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot invoke property." );
			return nullptr;
		}
	}

	// Return none to pass the test
	Py_RETURN_NONE;
}


/**
 *	Tests for converting an old-style Python class to a reflected object.

 *	@param self the reflection module.
 *	@param args first argument must be a Python class.
 *		e.g. reflection.conversionTest(testClass)
 *	@param argument keywords
 *		e.g. reflection.create(object=testClass)
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * py_oldStyleConversionTest( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );
	assert( g_definitionManager != nullptr );
	assert( g_typeConverters != nullptr );
	ReflectedPython::DefinedInstance instance( (*g_definitionManager),
		scriptObject,
		(*g_typeConverters) );

	auto pCommonResult = commonConversionTest( instance );
	return pCommonResult;
}


/**
 *	Tests for converting a new-style Python class to a reflected object.

 *	@param self the reflection module.
 *	@param args first argument must be a Python class.
 *		e.g. reflection.conversionTest(testClass)
 *	@param argument keywords
 *		e.g. reflection.create(object=testClass)
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * py_newStyleConversionTest( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );
	assert( g_definitionManager != nullptr );
	assert( g_typeConverters != nullptr );
	ReflectedPython::DefinedInstance instance( (*g_definitionManager),
		scriptObject,
		(*g_typeConverters) );

	auto pCommonResult = commonConversionTest( instance );
	if (pCommonResult == nullptr)
	{
		return pCommonResult;
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest1", stringTest );

		if (setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot set property." );
			return nullptr;
		}

		const std::string expectedString = "Read-only Property";
		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest1", stringCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (stringCheck != expectedString)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest2", stringTest );

		if (setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot set property." );
			return nullptr;
		}

		const std::string expectedString = "Read-only Property";
		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest2", stringCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (stringCheck != expectedString)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}

	{
		// @see descriptors __get__ and __set__
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"descriptorTest", stringTest );

		if (!setSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot set property." );
			return nullptr;
		}

		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"descriptorTest", stringCheck );

		if (!getSuccess)
		{
			PyErr_Format( PyExc_TypeError,
				"Cannot get property." );
			return nullptr;
		}
		if (stringTest != stringCheck)
		{
			PyErr_Format( PyExc_TypeError,
				"Got invalid property." );
			return nullptr;
		}
	}

	Py_RETURN_NONE;
}


} // namespace


ReflectionModule::ReflectionModule( IDefinitionManager& definitionManager,
	IObjectManager& objectManager,
	const PythonTypeConverters & typeConverters )
{
	g_definitionManager = &definitionManager;
	g_objectManager = &objectManager;
	g_typeConverters = &typeConverters;
	assert( Py_IsInitialized() );

	static PyMethodDef s_methods[] =
	{
		{
			"create",
			reinterpret_cast< PyCFunction >( &py_create ),
			METH_VARARGS|METH_KEYWORDS,
			"Create C++ reflected object and return it to Python"
		},
		{
			"oldStyleConversionTest",
			reinterpret_cast< PyCFunction >( &py_oldStyleConversionTest ),
			METH_VARARGS|METH_KEYWORDS,
			"Inspect an old-style Python class using the reflection system"
		},
		{
			"newStyleConversionTest",
			reinterpret_cast< PyCFunction >( &py_newStyleConversionTest ),
			METH_VARARGS|METH_KEYWORDS,
			"Inspect a new-style Python class using the reflection system"
		},
		{ nullptr, nullptr, 0, nullptr }
	};

	PyObject *m = Py_InitModule( "reflection", s_methods );
	assert( m != nullptr );
}

