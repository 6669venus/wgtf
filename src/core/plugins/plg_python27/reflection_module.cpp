#include "pch.hpp"

#include "reflection_module.hpp"
#include "core_reflection/i_object_manager.hpp"

#include <cassert>

namespace
{
IDefinitionManager* g_definitionManager = nullptr;
IObjectManager* g_objectManager = nullptr;

/**
 */
static PyObject * py_create( PyObject * self, PyObject * args, PyObject * kw )
{
	char * objectType = nullptr;

	static char *keywords [] = {
		"objectType"
	};

	if (!PyArg_ParseTupleAndKeywords( args, kw, "s", keywords, &objectType ))
	{
		PyErr_Format( PyExc_TypeError, "Could not parse arguments" );
		return NULL;
	}
	if (objectType[0] == '\0')
	{
		PyErr_Format( PyExc_TypeError,
			"Can not create a definition from an empty string." );
		return NULL;
	}

	assert( g_definitionManager != nullptr );
	const IClassDefinition * pDefinition =
		g_definitionManager->getDefinition( objectType );
	if (pDefinition == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Could not find a definition for \"%s\"\n", objectType );
		return NULL;
	}

	ObjectHandle object = pDefinition->create();
	assert( object != nullptr );

	// TODO

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
		{ "create", (PyCFunction)&py_create, METH_VARARGS|METH_KEYWORDS, "Create reflected object" },
		{ NULL, NULL, 0, NULL }
	};

	PyObject *m = Py_InitModule( "Reflection", s_methods );
	assert( m != nullptr );
}

