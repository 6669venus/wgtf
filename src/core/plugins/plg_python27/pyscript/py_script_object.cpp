#include "pch.hpp"

#include "Python.h"

#include "py_script_object.hpp"

BW_BEGIN_NAMESPACE

/* static */ bool ScriptModule::moduleExists( const char * name )
{
	MF_ASSERT( name );
	
	char buf[ BW_MAX_PATH ];

	FILE * fp = NULL;
	PyObject * pLoader = NULL;

	bool result = (_PyImport_FindModule( name, NULL, buf, sizeof( buf ),
		&fp, &pLoader ) != NULL);

	if (fp)
	{
		fclose( fp );
	}

	Py_XDECREF( pLoader );

	Script::clearError();

	return result;
}

BW_END_NAMESPACE

// py_script_object.cpp
