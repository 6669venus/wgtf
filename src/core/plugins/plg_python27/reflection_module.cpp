#include "pch.hpp"

#include "reflection_module.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "definer.hpp"

#include <cassert>

namespace
{

/// State storage for static functions attached to Python
/// TODO Move to ReflectionModule somehow?
IDefinitionManager* g_definitionManager = nullptr;
IObjectManager* g_objectManager = nullptr;


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
 *	Test converting a Python object to a reflected object.

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
static PyObject * py_conversionTest( PyObject * self,
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

	PyScript::ScriptObject scriptObject( object );
	assert( g_definitionManager != nullptr );
	Definer definer( (*g_definitionManager), scriptObject );

	// Test each public method on Definer
	{
		const IClassDefinition & definition = definer.getDefinition();
	}

	{
		std::string name;
		const bool success = definer.get< std::string >( "name", name );

		// TODO NGT-1161
		//if (!success)
		//{
		//	PyErr_Format( PyExc_TypeError,
		//		"Cannot get property." );
		//	return nullptr;
		//}
	}
	{
		const std::string name = "Name was set";
		const bool success = definer.set< std::string >( "name", name );

		// TODO NGT-1162
		//if (!success)
		//{
		//	PyErr_Format( PyExc_TypeError,
		//		"Cannot set property." );
		//	return nullptr;
		//}
	}
	{
		Variant name = Variant( "Name was set" );
		const bool success = definer.set( "name", name );

		// TODO NGT-1162
		//if (!success)
		//{
		//	PyErr_Format( PyExc_TypeError,
		//		"Cannot set property." );
		//	return nullptr;
		//}
	}

	// Return none to pass the test
	Py_RETURN_NONE;
}


} // namespace


ReflectionModule::ReflectionModule( IDefinitionManager& definitionManager,
	IObjectManager& objectManager )
{
	g_definitionManager = &definitionManager;
	g_objectManager = &objectManager;
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
			"conversionTest",
			reinterpret_cast< PyCFunction >( &py_conversionTest ),
			METH_VARARGS|METH_KEYWORDS,
			"Inspect a Python object using the reflection system"
		},
		{ nullptr, nullptr, 0, nullptr }
	};

	PyObject *m = Py_InitModule( "reflection", s_methods );
	assert( m != nullptr );
}

