#include "pch.hpp"

#include "interpreter.hpp"
#include "py_import_paths.hpp"
#include "py_script_object.hpp"

#include "osdefs.h" // Needed for DELIM

#include <string>

/**
 *	This method prints out the current Python callstack as debug messages.
 *	Based on:
 *	http://stackoverflow.com/questions/1796510/accessing-a-python-traceback-from-the-c-api
 */
//void Script::printStack()
//{
//	const PyThreadState * tstate = PyThreadState_GET();
//
//	if ((tstate != NULL) && (tstate->frame != NULL))
//	{
//		const PyFrameObject * frame = tstate->frame;
//
//		DEBUG_MSG( "Python stack trace:\n" );
//		while (frame != NULL)
//		{
//			const int line = frame->f_lineno;
//			const char * filename =
//				PyString_AsString( frame->f_code->co_filename );
//			const char * funcname =
//				PyString_AsString( frame->f_code->co_name );
//
//			DEBUG_MSG( "    %s(%d): %s\n", filename, line, funcname );
//
//			frame = frame->f_back;
//		}
//	}
//}


/**
 *	This static method initialises the scripting system.
 *	The paths must be separated with semicolons.
 */
bool Python27Interpreter::init()
{
	//if (s_isInitalised)
	//{
	//	return true;
	//}

	// Initialise python
	//Py_VerboseFlag = 2;
	Py_FrozenFlag = 1; // Suppress errors from getpath.c


	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	Py_NoSiteFlag = 1;
	Py_IgnoreEnvironmentFlag = 1;

	Py_Initialize();

	// TODO Hack
	static const std::wstring path( L"C:/git/ngt1/src/core/testing/plg_python27_test/scripts" );
	// Build the list of python code file paths relative to our IFileSystem
	//PyImportPaths sysPaths( DELIM );
	//// TODO Hack
	//sysPaths.append( "C:/git/ngt1/src/core/testing/plg_python27_test/scripts" );

	//PyObject * pSys = sysPaths.pathAsObject();
	PyObject *pSys;
	const size_t len = 1;
	const size_t i = 0;
	pSys = PyList_New( len );
	PyObject * pPath = PyUnicode_FromWideChar(
		const_cast<wchar_t *>( path.c_str() ), path.size() );
	if (pPath != NULL) 
	{
		PyList_SetItem(pSys, i, pPath);
	}

	int result = PySys_SetObject( "path", pSys );
	Py_DECREF( pSys );
	if (result != 0)
	{
		NGT_ERROR_MSG( "Script::init: Unable to assign sys.path\n" );
		return false;
	}

	return true;
}


/**
 *	This static method terminates the scripting system.
 */
void Python27Interpreter::fini()
{
	//if (!s_isInitalised)
	//{
	//	return;
	//}

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
