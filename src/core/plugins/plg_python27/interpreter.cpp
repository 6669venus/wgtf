#include "pch.hpp"

#include "interpreter.hpp"

namespace Python27Interpreter
{
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
bool init()
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

	return true;
}


/**
 *	This static method terminates the scripting system.
 */
void fini()
{
	//if (!s_isInitalised)
	//{
	//	return;
	//}

	Py_Finalize();
}


} // namespace Python27Interpreter

