#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "scripting_engine.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"

#include "core_reflection/object_handle.hpp"


Python27ScriptingEngine::Python27ScriptingEngine( IComponentContext& context )
	: context_( context )
{
}


Python27ScriptingEngine::~Python27ScriptingEngine()
{
}


bool Python27ScriptingEngine::init()
{
	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	// Disable importing Lib/site.py on startup.
	Py_NoSiteFlag = 1;
	// Enable debug output
	// Requires the scriptoutputwriter output hook from stdout/stderr
	//Py_VerboseFlag = 2;
	// Use environment variables
	Py_IgnoreEnvironmentFlag = 0;

	// Initialize logging as a standard module
	// Must be before Py_Initialize()
	PyImport_AppendInittab( "scriptoutputwriter",
		PyScript::PyInit_ScriptOutputWriter );

	Py_Initialize();
	
	// Import the logging module
	// Must be after Py_Initialize()
	PyImport_ImportModule( "scriptoutputwriter" );

	// Allow import from supported system modules
	if (!this->appendPath(
		L"..\\..\\..\\src\\core\\third_party\\python\\Python-2.7.10\\Lib" ))
	{
		NGT_ERROR_MSG( "Failed to append path to system modules\n" );
		return false;
	}

	return true;
}


void Python27ScriptingEngine::fini()
{
	// Must not use any PyObjects after this point
	Py_Finalize();
}


bool Python27ScriptingEngine::appendPath( const wchar_t* path )
{
	PyScript::ScriptObject testPathObject =
		PyScript::ScriptObject::createFrom( path );

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


ObjectHandle Python27ScriptingEngine::import( const char* name )
{
	if (!Py_IsInitialized())
	{
		return nullptr;
	}

	PyScript::ScriptModule module = PyScript::ScriptModule::import( name,
		PyScript::ScriptErrorPrint( "Unable to import\n" ) );

	if (!module.exists())
	{
		return nullptr;
	}

	std::unique_ptr<ReflectedPython::DefinedInstance> pointer(
		new ReflectedPython::DefinedInstance( context_, module ) );
	ObjectHandleT<ReflectedPython::DefinedInstance> handle( std::move( pointer ), &pointer->getDefinition() );
	return handle;
}


bool Python27ScriptingEngine::checkErrors()
{
	if (PyScript::Script::hasError())
	{
		PyScript::Script::printError();
		PyScript::Script::clearError();
		return true;
	}

	return false;
}
