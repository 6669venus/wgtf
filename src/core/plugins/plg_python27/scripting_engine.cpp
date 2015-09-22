#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "scripting_engine.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/type_converter.hpp"


bool Python27ScriptingEngine::init()
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

	return true;
}


void Python27ScriptingEngine::fini()
{
	Py_Finalize();
}


bool Python27ScriptingEngine::appendPath( const wchar_t* path )
{
	PyObject * pyTestPath = TypeConverter::getData( path );
	PyScript::ScriptObject testPathObject( pyTestPath );

	PyObject* pySysPaths = PySys_GetObject( "path" );
	if (pySysPaths == nullptr)
	{
		NGT_ERROR_MSG( "Unable to get sys.path\n" );
		return false;
	}

	PyScript::ScriptList sysPaths( pySysPaths );
	sysPaths.append( testPathObject );

	const int result = PySys_SetObject( "path", pySysPaths );
	if (result != 0)
	{
		NGT_ERROR_MSG( "Unable to assign sys.path\n" );
		return false;
	}

	return true;
}


bool Python27ScriptingEngine::import( const char* name )
{
	if (!Py_IsInitialized())
	{
		return false;
	}
	return (PyScript::ScriptModule::import( name,
		PyScript::ScriptErrorPrint( "Unable to import\n" ) ).get() != nullptr);
}
