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


void noneConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void numericConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void stringConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void childConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void listConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void tupleConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void methodConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );


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
	ReflectedPython::DefinedInstance & instance )
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
	noneConversionTest( instance, m_name, result_ );
	numericConversionTest( instance, m_name, result_ );
	stringConversionTest( instance, m_name, result_ );
	childConversionTest( instance, m_name, result_ );
	listConversionTest( instance, m_name, result_ );
	tupleConversionTest( instance, m_name, result_ );
	methodConversionTest( instance, m_name, result_ );

	// Return none to pass the test
	Py_RETURN_NONE;
}


void noneConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Convert Python None -> C++ nullptr
	{
		// @see Py_None
		void * noneType = nullptr;
		const bool setSuccess = instance.set< void * >(
			"noneTest", noneType );

		CHECK( setSuccess );

		void * noneResult;
		const bool getSuccess = instance.get< void * >(
			"noneTest", noneResult );

		CHECK( getSuccess );
		CHECK_EQUAL( noneType, noneResult );
	}
}


void numericConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyBoolObject
		const bool boolExpected = false;
		const bool setSuccess = instance.set< bool >( "boolTest", boolExpected );

		CHECK( setSuccess );

		bool boolResult = true;
		const bool getSuccess = instance.get< bool >( "boolTest", boolResult );

		CHECK( getSuccess );
		CHECK_EQUAL( boolExpected, boolResult );
	}
	{
		// @see PyIntObject
		const int intExpected = 2;
		const bool setSuccess = instance.set< int >( "intTest", intExpected );

		CHECK( setSuccess );

		int intResult = 1;
		const bool getSuccess = instance.get< int >( "intTest", intResult );

		CHECK( getSuccess );
		CHECK_EQUAL( intExpected, intResult );
	}
	// Check class properties
	{
		// @see PyIntObject
		const int classIntExpected = 2;
		const bool setSuccess = instance.set< int >( "classIntTest", classIntExpected );

		CHECK( setSuccess );

		int classIntResult = 1;
		const bool getSuccess = instance.get< int >( "classIntTest", classIntResult );

		CHECK( getSuccess );
		CHECK_EQUAL( classIntExpected, classIntResult );
	}
	{
		// @see PyLongObject
		const digit longExpected = 2;
		const bool setSuccess = instance.set< digit >( "longTest", longExpected );

		CHECK( setSuccess );

		digit longResult = 1;
		const bool getSuccess = instance.get< digit >( "longTest", longResult );

		CHECK( getSuccess );
		CHECK_EQUAL( longExpected, longResult );
	}
	{
		// @see PyFloatObject
		const double floatExpected = 2.0;
		const bool setSuccess = instance.set< double >( "floatTest", floatExpected );

		CHECK( setSuccess );

		double floatCheck = 1.0;
		const bool getSuccess = instance.get< double >( "floatTest", floatCheck );

		CHECK( getSuccess );
		// TODO direct floating point comparison is bad
		CHECK_EQUAL( floatExpected, floatCheck );
	}
	// TODO structs
	//{
	//	// @see PyComplexObject
	//	Py_complex complexExpected;
	//	complexExpected.real = 1.0;
	//	complexExpected.imag = 0.0;
	//	const bool setSuccess = instance.set< Py_complex >(
	//		"complexExpected", complexExpected );

	//	CHECK( setSuccess );

	//	Py_complex complexResult;
	//	complexResult.real = 0.0;
	//	complexResult.imag = 1.0;
	//	const bool getSuccess = instance.get< Py_complex >(
	//		"complexExpected", complexResult );

	//	CHECK( getSuccess );
	//	// TODO direct floating point comparison is bad
	//	CHECK( (complexExpected.real == complexResult.real) &&
	//		(complexExpected.imag == complexResult.imag) );
	//}
}


void stringConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyStringObject
		const std::string stringExpected = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"stringTest", stringExpected );

		CHECK( setSuccess );

		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"stringTest", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( stringExpected, stringResult );
	}
	// TODO causes memory leak
	//{
	//	const std::wstring unicodeExpected = L"String was set";
	//	const bool setSuccess = instance.set< std::wstring >(
	//		"unicodeExpected", unicodeExpected );
	//	CHECK( setSuccess );

	//	std::wstring unicodeResult = L"Fail";
	//	const bool getSuccess = instance.get< std::wstring >(
	//		"unicodeExpected", unicodeResult );

	//	CHECK( getSuccess );
	//	CHECK_EQUAL( unicodeExpected, unicodeResult );
	//}
}


void childConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see object in object
		const std::string childPropertyExpected = "Child";
		const bool setSuccess = instance.set< std::string >(
			"childTest.stringTest", childPropertyExpected );

		CHECK( setSuccess );

		std::string childPropertyResult;
		const bool getSuccess = instance.get< std::string >(
			"childTest.stringTest", childPropertyResult );

		CHECK( getSuccess );
		CHECK_EQUAL( childPropertyExpected, childPropertyResult );
	}
}


void resetList( ReflectedPython::DefinedInstance& instance,
	const size_t size,
	const char * m_name,
	TestResult & result_ )
{
	// Reset list in case another test above modified it
	std::vector< Variant > container;
	container.reserve( size );
	for (int i = 0; i < static_cast< int >( container.capacity() ); ++i)
	{
		container.emplace_back( i );
	}
	Collection listTest( container );
	const bool resetSuccess = instance.set< Collection >(
		"listTest", listTest );

	CHECK( resetSuccess );
}


void checkList( const Collection & listResult,
	size_t expectedSize,
	const char * m_name,
	TestResult & result_ )
{
	int i = 0;
	CHECK_EQUAL( expectedSize, listResult.size() );
	for (const auto & item : listResult)
	{
		int value = -1;
		const bool success = item.tryCast( value );
		CHECK( success );
		CHECK_EQUAL( i, value );
		++i;
	}
}


void listConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyListObject
		const size_t expectedSize = 10;
		resetList( instance, expectedSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );

		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// First element
		const int listExpected = 10;
		const bool setSuccess = instance.set< int >(
			"listTest[0]", listExpected );

		CHECK( setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[0]", listResult );

		CHECK( getSuccess );
		CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Second element
		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[1]", listExpected );

		CHECK( setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[1]", listResult );

		CHECK( getSuccess );
		CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Out-of-range
		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[999]", listExpected );

		CHECK( !setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[999]", listResult );

		CHECK( !getSuccess );
	}
	{
		// @see PyListObject
		// Last item in list

		// Reset list in case another test above modified it
		const size_t expectedSize = 5;
		resetList( instance, expectedSize, m_name, result_ );

		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[-1]", listExpected );

		CHECK( setSuccess );

		{
			int listResult = 0;
			const bool getSuccess = instance.get< int >(
				"listTest[-1]", listResult );

			CHECK( getSuccess );
			CHECK_EQUAL( listExpected, listResult );
		}
		{
			int listResult = 0;
			const bool getSuccess = instance.get< int >(
				"listTest[4]", listResult );

			CHECK( getSuccess );
			CHECK_EQUAL( listExpected, listResult );
		}
	}
	{
		// @see PyListObject
		// Negative out-of-range

		// Reset list in case another test above modified it
		const size_t expectedSize = 5;
		resetList( instance, expectedSize, m_name, result_ );

		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[-100]", listExpected );

		CHECK( !setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[-100]", listResult );

		CHECK( !getSuccess );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range 0-3
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[0:3]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range 1-3, with a step of 1
		//// i.e. set 1, 2, 3
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[1:3:1]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range first-last, with a step of 2
		//// i.e. set 0, 2, 4
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[::2]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Append to end
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const size_t insertionSize = 5;
		for (int i = 0; i < static_cast< int >( originalSize ); ++i)
		{
			Variant position( i + originalSize );
			auto insertionItr = listResult.insert( position );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( position );
		}

		const size_t expectedSize = (originalSize + insertionSize);
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Insert in middle
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int insertionPosition = 2;
		{
			Variant position( insertionPosition );
			auto insertionItr = listResult.insert( position );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( position );
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i <= insertionPosition)
				{
					CHECK_EQUAL( i, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyListObject
		// Insert at start
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int insertionPosition = -100;
		{
			Variant position( insertionPosition );
			auto insertionItr = listResult.insert( position );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( position );
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i == 0)
				{
					CHECK_EQUAL( insertionPosition, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyListObject
		// Erase existing item by key
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = originalSize - 1;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		sprintf( buffer, "%d", erasureId );
		Variant key( buffer );
		auto erasureCount = listResult.erase( key );
		CHECK( erasureCount == 1 );

		const size_t expectedSize = originalSize - 1;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase with invalid key
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		Variant key( "Invalid" );
		auto erasureCount = listResult.erase( key );
		CHECK( erasureCount == 0 );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase existing item by iterator
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = 2;
		{
			const size_t maxDigits = 10;
			char buffer[ maxDigits ];
			sprintf( buffer, "%d", erasureId );
			const Variant key( buffer );
			auto itr = listResult.find( key );
			CHECK( itr != listResult.end() );
			auto erasureItr = listResult.erase( itr );
			CHECK( erasureItr != listResult.end() );
		}

		const size_t expectedSize = originalSize - 1;
		CHECK_EQUAL( expectedSize, listResult.size() );

		int i = 0;
		for (const auto & item : listResult)
		{
			const int expectedValue = (i < erasureId) ? i : i + 1;
			int value = -1;
			const bool success = item.tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedValue, value );
			++i;
		}
	}
	{
		// @see PyListObject
		// Erase by invalid iterator
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		{
			auto itr = listResult.end();
			auto erasureItr = listResult.erase( itr );
			CHECK( erasureItr == listResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase existing item by iterator range
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t startId = 1;
		const size_t endId = 3;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = listResult.find( startKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = listResult.find( endKey );
		CHECK( endItr != listResult.end() );

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr != listResult.end() );
		{
			const int expectedPosition = 1;
			int value = -1;
			const bool success = erasureItr.key().tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedPosition, value );
		}

		const size_t expectedSize = originalSize - (endId - startId);
		CHECK_EQUAL( expectedSize, listResult.size() );

		int i = 0;
		for (const auto & item : listResult)
		{
			if ((i >= startId) && (i < endId))
			{
				continue;
			}
			const int expectedValue = (i < startId) ? i : i + (endId - startId);
			int value = -1;
			const bool success = item.tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedValue, value );
			++i;
		}
	}
	{
		// @see PyListObject
		// Erase existing item by iterator range of size 0
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = 1;
		const Variant erasureKey( erasureId );

		const auto startItr = listResult.find( erasureKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = startItr;

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase entire list
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t startId = 0;
		const size_t endId = originalSize;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = listResult.find( startKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = listResult.find( endKey );
		CHECK( endItr == listResult.end() );

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		CHECK( listResult.empty() );
	}
	{
		// @see PyListObject
		// Erase existing item by invalid range
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const auto startItr = listResult.end();
		const auto endItr = listResult.end();

		const auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Get existing with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = 2;
		Variant position( getPosition );
		auto valueRef = listResult[ position ];

		int result = 0;
		const bool success = valueRef.tryCast< int >( result );
		CHECK( success );
		CHECK( result == getPosition );
	}
	{
		// @see PyListObject
		// Insert at end with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = originalSize;
		{
			Variant position( getPosition );
			auto valueRef = listResult[ position ];

			// Check it inserted None
			void * result = static_cast< void * >( &position );
			const bool success = valueRef.tryCast< void * >( result );
			CHECK( success );
			CHECK( result == nullptr );

			// Set value to int
			valueRef = getPosition;
		}

		const size_t expectedSize = (originalSize + 1);
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Insert at start with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = -static_cast< int >( originalSize ) - 1;
		{
			Variant position( getPosition );
			auto valueRef = listResult[ position ];

			// Check it inserted None
			void * result = static_cast< void * >( &position );
			const bool success = valueRef.tryCast< void * >( result );
			CHECK( success );
			CHECK( result == nullptr );

			// Set value to int
			valueRef = getPosition;
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i == 0)
				{
					CHECK_EQUAL( getPosition, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyTupleObject
		// Tuple containing different types
		const size_t originalSize = 5;
		{
			std::vector< Variant > container;
			container.reserve( originalSize );
			container.emplace_back( 0 );
			container.emplace_back( 1 );
			container.emplace_back( 2 );
			container.emplace_back( "Hello" );
			container.emplace_back( "World" );
			Collection listTest( container );
			const bool resetSuccess = instance.set< Collection >(
				"listTest", listTest );

			CHECK( resetSuccess );
		}

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );
		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const size_t expectedSize = originalSize;
		CHECK_EQUAL( expectedSize, listResult.size() );
		int i = 0;
		for (const auto & item : listResult)
		{
			if (i < 3)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( i, value );
			}
			else if (i == 3)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "Hello", value );
			}
			else if (i == 4)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "World", value );
			}
			++i;
		}
	}
}


template< size_t size >
void resetTuple( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Reset list in case another test above modified it
	std::array< Variant, size > container;
	for (int i = 0; i < static_cast< int >( container.max_size() ); ++i)
	{
		container[ i ] = i;
	}
	Collection tupleTest( container );
	const bool resetSuccess = instance.set< Collection >(
		"tupleTest", tupleTest );

	CHECK( resetSuccess );
}


void checkTuple( const Collection & tupleResult,
	size_t expectedSize,
	const char * m_name,
	TestResult & result_ )
{
	int i = 0;
	CHECK_EQUAL( expectedSize, tupleResult.size() );
	for (const auto & item : tupleResult)
	{
		int value = -1;
		const bool success = item.tryCast( value );
		CHECK( success );
		CHECK_EQUAL( i, value );
		++i;
	}
}


void tupleConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyTupleObject
		const size_t expectedSize = 10;
		resetTuple< expectedSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );

		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// First element
		const int tupleExpected = 10;
		const bool setSuccess = instance.set< int >(
			"tupleTest[0]", tupleExpected );

		CHECK( setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[0]", tupleResult );

		CHECK( getSuccess );
		CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Second element
		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[1]", tupleExpected );

		CHECK( setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[1]", tupleResult );

		CHECK( getSuccess );
		CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Out-of-range
		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[999]", tupleExpected );

		CHECK( !setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[999]", tupleResult );

		CHECK( !getSuccess );
	}
	{
		// @see PyTupleObject
		// Last item in tuple

		// Reset tuple in case another test above modified it
		const size_t expectedSize = 5;
		resetTuple< expectedSize >( instance, m_name, result_ );

		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[-1]", tupleExpected );

		CHECK( setSuccess );

		{
			int tupleResult = 0;
			const bool getSuccess = instance.get< int >(
				"tupleTest[-1]", tupleResult );

			CHECK( getSuccess );
			CHECK_EQUAL( tupleExpected, tupleResult );
		}
		{
			int tupleResult = 0;
			const bool getSuccess = instance.get< int >(
				"tupleTest[4]", tupleResult );

			CHECK( getSuccess );
			CHECK_EQUAL( tupleExpected, tupleResult );
		}
	}
	{
		// @see PyTupleObject
		// Negative out-of-range

		// Reset tuple in case another test above modified it
		const size_t expectedSize = 5;
		resetTuple< expectedSize >( instance, m_name, result_ );

		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[-100]", tupleExpected );

		CHECK( !setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[-100]", tupleResult );

		CHECK( !getSuccess );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range 0-3
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[0:3]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range 1-3, with a step of 1
		//// i.e. set 1, 2, 3
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[1:3:1]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range first-last, with a step of 2
		//// i.e. set 0, 2, 4
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[::2]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Append to end
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		Variant position( originalSize + 1 );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert in middle
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int insertionPosition = 2;
		Variant position( insertionPosition );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert at start
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int insertionPosition = -100;
		Variant position( insertionPosition );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by key
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t erasureId = originalSize - 1;
		Variant key( erasureId );
		auto erasureCount = tupleResult.erase( key );
		CHECK( erasureCount == 0 );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by iterator
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t erasureId = 2;
		{
			const Variant key( erasureId );
			auto itr = tupleResult.find( key );
			CHECK( itr != tupleResult.end() );
			auto erasureItr = tupleResult.erase( itr );
			CHECK( erasureItr == tupleResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase by invalid iterator
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		{
			auto itr = tupleResult.end();
			auto erasureItr = tupleResult.erase( itr );
			CHECK( erasureItr == tupleResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by iterator range
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t startId = 1;
		const size_t endId = 3;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = tupleResult.find( startKey );
		CHECK( startItr != tupleResult.end() );

		const auto endItr = tupleResult.find( endKey );
		CHECK( endItr != tupleResult.end() );

		auto erasureItr = tupleResult.erase( startItr, endItr );
		CHECK( erasureItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Get existing with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = 2;
		Variant position( getPosition );
		auto valueRef = tupleResult[ position ];

		int result = 0;
		const bool success = valueRef.tryCast< int >( result );
		CHECK( success );
		CHECK( result == getPosition );
	}
	{
		// @see PyTupleObject
		// Insert at end with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = originalSize;
		{
			Variant position( getPosition );
			auto valueRef = tupleResult[ position ];

			// Check it returns end (did not insert)
			CHECK( Variant( valueRef ).isVoid() );

			// Set value to int
			valueRef = getPosition;

			// Check it returns end (did not set)
			CHECK( Variant( valueRef ).isVoid() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert at start with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = -static_cast< int >( originalSize ) - 1;
		{
			Variant position( getPosition );
			auto valueRef = tupleResult[ position ];

			// Check it returns end (did not insert)
			CHECK( Variant( valueRef ).isVoid() );

			// Set value to int
			valueRef = getPosition;

			// Check it returns end (did not set)
			CHECK( Variant( valueRef ).isVoid() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Tuple containing different types
		const size_t originalSize = 5;
		{
			std::array< Variant, originalSize > container;
			container[ 0 ] = 0;
			container[ 1 ] = 1;
			container[ 2 ] = 2;
			container[ 3 ] = "Hello";
			container[ 4 ] = "World";
			Collection tupleTest( container );
			const bool resetSuccess = instance.set< Collection >(
				"tupleTest", tupleTest );

			CHECK( resetSuccess );
		}

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );
		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const size_t expectedSize = originalSize;
		CHECK_EQUAL( expectedSize, tupleResult.size() );
		int i = 0;
		for (const auto & item : tupleResult)
		{
			if (i < 3)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( i, value );
			}
			else if (i == 3)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "Hello", value );
			}
			else if (i == 4)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "World", value );
			}
			++i;
		}
	}
}


void methodConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "methodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Method test was run", returnValue );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "classMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Class method test was run", returnValue );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "staticMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Static method test was run", returnValue );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest1", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Function test was run", returnValue );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest2", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Callable class test was run", returnValue );
	}
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
		const char * typeExpected = "__builtin__.classobj";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python type <- C++ TypeId
	{
		PythonMetaType intType( PyScript::ScriptType( &PyInt_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) );
		const bool setSuccess = instance.set< PythonMetaType >(
			"typeTest1", intType );

		CHECK( setSuccess );

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), intType.name() ) );
	}
	// Convert Python type -> C++ TypeId
	{
		// @see types.TypeType
		const char * typeExpected = "__builtin__.type";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest2", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.ClassType
		const char * typeExpected = "python27_test.OldClassTest";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.ClassType
		const char * typeExpected = "python27_test.OldClassTest";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest2", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python instance -> C++ TypeId
	{
		// @see types.InstanceType
		const char * typeExpected = "__builtin__.instance";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"instanceTest", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
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
		const char * typeExpected = "__builtin__.type";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python type <- C++ TypeId
	{
		PythonMetaType intType( PyScript::ScriptType( &PyInt_Type,
			PyScript::ScriptObject::FROM_BORROWED_REFERENCE ) );
		const bool setSuccess = instance.set< PythonMetaType >(
			"typeTest1", intType );

		CHECK( setSuccess );

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), intType.name() ) );
	}
	// Convert Python type -> C++ TypeId
	{
		// @see types.TypeType
		const char * typeExpected = "__builtin__.type";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"typeTest2", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.TypeType
		const char * typeExpected = "python27_test.NewClassTest";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest1", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python class -> C++ TypeId
	{
		// @see types.TypeType
		const char * typeExpected = "python27_test.NewClassTest";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"classTest2", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	// Convert Python instance -> C++ TypeId
	{
		// @see types.TypeType
		const char * typeExpected = "python27_test.NewClassTest";

		PythonMetaType typeResult;
		const bool getSuccess = instance.get< PythonMetaType >(
			"instanceTest", typeResult );

		CHECK( getSuccess );
		CHECK_EQUAL( 0, strcmp( typeResult.name(), typeExpected ) );
	}
	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest1", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest1", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringResult );
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest2", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest2", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringResult );
	}

	{
		// @see descriptors __get__ and __set__
		const std::string stringExpected = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"descriptorTest", stringExpected );

		CHECK( setSuccess );

		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"descriptorTest", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( stringExpected, stringResult );
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

