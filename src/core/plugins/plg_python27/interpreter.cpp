#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "interpreter.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


bool Python27Interpreter::init()
{
	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	// Disable importing Lib/site.py on startup.
	Py_NoSiteFlag = 1;
	// Debugging TODO needs output hook from stderr to OutputDebugString
	//Py_VerboseFlag = 2;
	// Use environment variables
	Py_IgnoreEnvironmentFlag = 0;

	Py_Initialize();

	// TODO allow plugins to register their paths to scripts
	std::wstring testPath(
	//	L"C:\\git\\ngt1\\src\\core\\testing\\plg_python27_test\\scripts" );
		L"..\\..\\..\\src\\core\\testing\\plg_python27_test\\scripts" );
	PyObject * pyTestPath = TypeConverter::getData( testPath );
	PyScript::ScriptObject testPathObject( pyTestPath );

	PyScript::ScriptList sysPaths = PyScript::ScriptList::create();
	sysPaths.append( testPathObject );

	PyObject * pSys = sysPaths.get();
	int result = PySys_SetObject( "path", pSys );
	//Py_DECREF( pSys );
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
	return (PyScript::ScriptModule::import( name,
		PyScript::ScriptErrorPrint( "Unable to import\n" ) ).get() != nullptr);
}
