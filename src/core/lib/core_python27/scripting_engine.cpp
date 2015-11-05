#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "module.hpp"
#include "scripting_engine.hpp"
#include "type_converters/python_meta_type.hpp"

#include "core_variant/interfaces/i_meta_type_manager.hpp"
#include "core_variant/variant.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"


#include <array>
#include <vector>


Python27ScriptingEngine::Python27ScriptingEngine()
	: pTypeConvertersInterface_( nullptr )
{
}


Python27ScriptingEngine::~Python27ScriptingEngine()
{
}


bool Python27ScriptingEngine::init( IComponentContext & context )
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

	// Register Python types to be usable by Variant
	auto pMetaTypeManager = context.queryInterface< IMetaTypeManager >();
	assert( pMetaTypeManager != nullptr );
	if (pMetaTypeManager != nullptr)
	{
		defaultMetaTypes_.emplace_back( new MetaTypeImpl< PythonMetaType >() );
		for (const auto & type : defaultMetaTypes_)
		{
			const auto success = pMetaTypeManager->registerType( type.get() );
			assert( success );
		}
	}

	// Register type converters for converting between PyObjects and Variant
	typeConverters_.registerTypeConverter( defaultTypeConverter_ );
	typeConverters_.registerTypeConverter( sequenceTypeConverter_ );
	typeConverters_.registerTypeConverter( typeTypeConverter_ );
	typeConverters_.registerTypeConverter( longTypeConverter_ );
	const bool transferOwnership = false;
	pTypeConvertersInterface_ = context.registerInterface(
		&typeConverters_,
		transferOwnership,
		IComponentContext::Reg_Local );

	return true;
}


void Python27ScriptingEngine::fini( IComponentContext & context )
{
	// Deregister type converters for converting between PyObjects and Variant
	typeConverters_.deregisterTypeConverter( longTypeConverter_ );
	typeConverters_.deregisterTypeConverter( typeTypeConverter_ );
	typeConverters_.deregisterTypeConverter( sequenceTypeConverter_ );
	typeConverters_.deregisterTypeConverter( defaultTypeConverter_ );
	context.deregisterInterface( pTypeConvertersInterface_ );

	// Register Python types to be usable by Variant
	auto pMetaTypeManager =
		context.queryInterface< IMetaTypeManager >();
	assert( pMetaTypeManager != nullptr );
	if (pMetaTypeManager != nullptr)
	{
		for (const auto & type : defaultMetaTypes_)
		{
			const auto success = pMetaTypeManager->deregisterType( type.get() );
			assert( success );
		}
	}
	defaultMetaTypes_.clear();

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


std::shared_ptr< IPythonModule > Python27ScriptingEngine::import(
	const char* name )
{
	if (!Py_IsInitialized())
	{
		return false;
	}

	PyScript::ScriptModule module = PyScript::ScriptModule::import( name,
		PyScript::ScriptErrorPrint( "Unable to import\n" ) );

	if (module.exists())
	{
		return std::make_shared< Python27Module >( Python27Module( module ) );
	}

	return nullptr;
}

