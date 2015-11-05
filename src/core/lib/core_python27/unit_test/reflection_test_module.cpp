#include "pch.hpp"

#include <longintrepr.h>

#include "reflection_test_module.hpp"

#include "core_python27/definition_details.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/type_converters/python_meta_type.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/type_class_definition.hpp"


namespace
{

/// State storage for static functions attached to Python
static ReflectionTestModule * g_module = nullptr;


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
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

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

	auto pDefinitionManager =
		g_module->context_.queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Could not get definition manager." );
		return nullptr;
	}

	const IClassDefinition * pDefinition =
		pDefinitionManager->getDefinition( objectType );
	if (pDefinition == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Could not find a definition for \"%s\"\n", objectType );
		return nullptr;
	}

	ObjectHandle object = pDefinition->create();
	CHECK( object != nullptr );

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
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

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


	// Convert Python None -> C++ nullptr
	{
		// @see Py_None
		PythonMetaType noneType( PyScript::ScriptType( Py_None,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) );
		const bool setSuccess = instance.set< PythonMetaType >(
			"noneTest", noneType );

		CHECK( setSuccess );

		PythonMetaType noneCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"noneTest", noneCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( noneType, noneCheck );
	}
	{
		// @see PyBoolObject
		const bool boolTest = false;
		const bool setSuccess = instance.set< bool >( "boolTest", boolTest );

		CHECK( setSuccess );

		bool boolCheck = true;
		const bool getSuccess = instance.get< bool >( "boolTest", boolCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( boolTest, boolCheck );
	}
	{
		// @see PyIntObject
		const int intTest = 2;
		const bool setSuccess = instance.set< int >( "intTest", intTest );

		CHECK( setSuccess );

		int intCheck = 1;
		const bool getSuccess = instance.get< int >( "intTest", intCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( intTest, intCheck );
	}
	// Check class properties
	{
		// @see PyIntObject
		const int classIntTest = 2;
		const bool setSuccess = instance.set< int >( "classIntTest", classIntTest );

		CHECK( setSuccess );

		int classIntCheck = 1;
		const bool getSuccess = instance.get< int >( "classIntTest", classIntCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( classIntTest, classIntCheck );
	}
	{
		// @see PyLongObject
		const digit longTest = 2;
		const bool setSuccess = instance.set< digit >( "longTest", longTest );

		CHECK( setSuccess );

		digit longCheck = 1;
		const bool getSuccess = instance.get< digit >( "longTest", longCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( longTest, longCheck );
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

		CHECK( getSuccess );
		// TODO direct floating point comparison is bad
		CHECK_EQUAL( floatTest, floatCheck );
	}
	// TODO structs
	//{
	//	// @see PyComplexObject
	//	Py_complex complexTest;
	//	complexTest.real = 1.0;
	//	complexTest.imag = 0.0;
	//	const bool setSuccess = instance.set< Py_complex >(
	//		"complexTest", complexTest );

	//	CHECK( setSuccess );

	//	Py_complex complexCheck;
	//	complexCheck.real = 0.0;
	//	complexCheck.imag = 1.0;
	//	const bool getSuccess = instance.get< Py_complex >(
	//		"complexTest", complexCheck );

	//	CHECK( getSuccess );
	//	// TODO direct floating point comparison is bad
	//	CHECK( (complexTest.real == complexCheck.real) &&
	//		(complexTest.imag == complexCheck.imag) );
	//}
	{
		// @see PyStringObject
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"stringTest", stringTest );

		CHECK( setSuccess );

		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"stringTest", stringCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( stringTest, stringCheck );
	}
	// TODO causes memory leak
	//{
	//	const std::wstring unicodeTest = L"String was set";
	//	const bool setSuccess = instance.set< std::wstring >(
	//		"unicodeTest", unicodeTest );
	//	CHECK( setSuccess );

	//	std::wstring unicodeCheck = L"Fail";
	//	const bool getSuccess = instance.get< std::wstring >(
	//		"unicodeTest", unicodeCheck );

	//	CHECK( getSuccess );
	//	CHECK_EQUAL( unicodeTest, unicodeCheck );
	//}
	{
		// @see object in object
		const std::string childPropertyTest = "Child";
		const bool setSuccess = instance.set< std::string >(
			"childTest.stringTest", childPropertyTest );

		CHECK( setSuccess );

		std::string childPropertyCheck;
		const bool getSuccess = instance.get< std::string >(
			"childTest.stringTest", childPropertyCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( childPropertyTest, childPropertyCheck );
	}
	{
		// @see PyTupleObject
		//std::array< Variant, 2 > tupleTest;
		//tupleTest[0] = Variant( "Item 1" );
		//tupleTest[1] = Variant( "Item 2" );
		//const bool setSuccess = instance.set< std::vector< Variant > >(
		//	"tupleTest", tupleTest );

		//CHECK( setSuccess );

		// TODO Variable size?
		//std::array< Variant, 2 > tupleCheck;
		//const bool getSuccess = instance.get< std::vector< Variant > >(
		//	"tupleTest", tupleCheck );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleTest, tupleCheck );
	}
	{
		// @see PyListObject
		//std::vector< Variant > listTest;
		//listTest.emplace_back( Variant( "Item 1" ) );
		//listTest.emplace_back( Variant( "Item 2" ) );
		//const bool setSuccess = instance.set< std::vector< Variant > >(
		//	"listTest", listTest );

		//CHECK( setSuccess );

		Collection listCheck;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listCheck );

		CHECK( getSuccess );

		int i = 0;
		const size_t expectedSize = 4;
		CHECK_EQUAL( expectedSize, listCheck.size() );
		for (const auto & item : listCheck)
		{
			std::string value;
			const bool success = item.tryCast( value );
			CHECK( success );
			++i;
		}
		//CHECK_EQUAL( listTest, listCheck );
	}
	{
		// @see PyListObject
		// first element
		//const int listTest = 10;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[0]", listTest );

		//CHECK( setSuccess );

		//int listCheck = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[0]", listCheck );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listTest, listCheck );
	}
	{
		// @see PyListObject
		// second element
		//const int listTest = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[1]", listTest );

		//CHECK( setSuccess );

		//int listCheck = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[1]", listCheck );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listTest, listCheck );
	}
	{
		// @see PyListObject
		// out-of-range
		//const int listTest = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[999]", listTest );

		//CHECK( !setSuccess );

		//int listCheck = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[999]", listCheck );

		//CHECK( !getSuccess );
	}
	{
		// @see PyDictObject
		//std::map< Variant, Variant > dictTest;
		//dictTest.insert( std::make_pair( Variant( "Key 1" ), Variant( "Item 1" ) ) );
		//dictTest.insert( std::make_pair( Variant( "Key 2" ), Variant( "Item 2" ) ) );
		//const bool setSuccess = instance.set< std::map< Variant, Variant > >(
		//	"dictTest", dictTest );

		//CHECK( setSuccess );

		//std::map< Variant, Variant > dictCheck;
		//const bool getSuccess = instance.get< std::map< Variant, Variant > >(
		//	"dictTest", dictCheck );

		//CHECK( getSuccess );
		//CHECK_EQUAL( dictTest, dictCheck );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "methodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( returnValue, "Method test was run" );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "classMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( returnValue, "Class method test was run" );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "staticMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( returnValue, "Static method test was run" );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest1", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( returnValue, "Function test was run" );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest2", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( returnValue, "Callable class test was run" );
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
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );

	ReflectedPython::DefinedInstance instance( g_module->context_,
		scriptObject );

	auto pCommonResult = commonConversionTest( instance );
	if (pCommonResult == nullptr)
	{
		return pCommonResult;
	}

	// Convert Python type -> C++ TypeId
	{
		// @see types.ClassType
		const char * getExpected = "__builtin__.classobj";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python type <- C++ TypeId
	{
		PythonMetaType intType( PyScript::ScriptType( &PyInt_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) );
		const bool setSuccess = instance.set< PythonMetaType >(
			"typeTest1", intType );

		CHECK( setSuccess );

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), intType.name() ), 0 );
	}
	// Convert Python type -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "__builtin__.type";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest2", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.ClassType
		const char * getExpected = "python27_test.OldClassTest";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.ClassType
		const char * getExpected = "python27_test.OldClassTest";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest2", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python instance -> C++ TypeId
	{
		// @see types.InstanceType
		const char * getExpected = "__builtin__.instance";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"instanceTest", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}

	Py_RETURN_NONE;
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
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );

	ReflectedPython::DefinedInstance instance( g_module->context_,
		scriptObject );

	auto pCommonResult = commonConversionTest( instance );
	if (pCommonResult == nullptr)
	{
		return pCommonResult;
	}

	// Convert Python type -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "__builtin__.type";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python type <- C++ TypeId
	{
		PythonMetaType intType( PyScript::ScriptType( &PyInt_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) );
		const bool setSuccess = instance.set< PythonMetaType >(
			"typeTest1", intType );

		CHECK( setSuccess );

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), intType.name() ), 0 );
	}
	// Convert Python type -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "__builtin__.type";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest2", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "python27_test.NewClassTest";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest1", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "python27_test.NewClassTest";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest2", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	// Convert Python instance -> C++ TypeId
	{
		// @see types.TypeType
		const char * getExpected = "python27_test.NewClassTest";

		PythonMetaType typeCheck;
		const bool getSuccess = instance.get< PythonMetaType >(
			"instanceTest", typeCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( strcmp( typeCheck.name(), getExpected ), 0 );
	}
	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest1", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest1", stringCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringCheck );
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest2", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest2", stringCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringCheck );
	}

	{
		// @see descriptors __get__ and __set__
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"descriptorTest", stringTest );

		CHECK( setSuccess );

		std::string stringCheck;
		const bool getSuccess = instance.get< std::string >(
			"descriptorTest", stringCheck );

		CHECK( getSuccess );
		CHECK_EQUAL( stringTest, stringCheck );
	}

	Py_RETURN_NONE;
}


} // namespace


ReflectionTestModule::ReflectionTestModule( IComponentContext & context,
	const char * testName,
	TestResult & result )
	: context_( context )
	, testName_( testName )
	, result_( result )
{
	g_module = this;

	const char * m_name = testName_;

	CHECK( Py_IsInitialized() );
	if (Py_IsInitialized())
	{
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

		PyObject *m = Py_InitModule( "reflectiontest", s_methods );
		CHECK( m != nullptr );
	}
}


ReflectionTestModule::~ReflectionTestModule()
{
	g_module = nullptr;
}

