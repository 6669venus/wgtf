#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "interpreter.hpp"
#include "pyscript/py_script_object.hpp"
#include "type_converter.hpp"


bool Python27Interpreter::init()
{
	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	// Disable importing Lib/site.py on startup.
	Py_NoSiteFlag = 1;

	Py_Initialize();

	// TODO allow plugins to register their paths to scripts
	std::wstring testPath(
		L"C:/git/ngt1/src/core/testing/plg_python27_test/scripts" );
	PyObject * pyTestPath = TypeConverter::getData( testPath );
	ScriptObject testPathObject( pyTestPath );

	ScriptList sysPaths;
	sysPaths.append( testPathObject );

	PyObject * pSys = sysPaths.get();
	int result = PySys_SetObject( "path", pSys );
	Py_DECREF( pSys );
	if (result != 0)
	{
		NGT_ERROR_MSG( "Python27Interpreter::init: Unable to assign sys.path\n" );
		return false;
	}

	return true;
}


void Python27Interpreter::fini()
{
	Py_Finalize();
}


bool Python27Interpreter::import( const char* name )
{
	if (!Py_IsInitialized())
	{
		return false;
	}
	return (ScriptModule::import( name,
		ScriptErrorPrint( "Unable to import\n" ) ).get() != nullptr);
}
