#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "module.hpp"
#include "reflection_module.hpp"
#include "scripting_engine.hpp"
#include "type_converters/string_type_converter.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"
#include "wg_pyscript/type_converter.hpp"


#include <array>
#include <vector>


class Python27ScriptingEngine::Implementation
{
public:
	Implementation();
	std::array< std::unique_ptr< IPythonTypeConverter >, 1 > defaultTypeConverters_;
	std::vector< IPythonTypeConverter * > typeConverters_;
};


Python27ScriptingEngine::Implementation::Implementation()
{
	defaultTypeConverters_[0].reset( new StringTypeConverter() );
}


Python27ScriptingEngine::Python27ScriptingEngine()
	: Implements< IPythonScriptingEngine >()
	, impl_( new Implementation() )
{
}


Python27ScriptingEngine::~Python27ScriptingEngine()
{
}


bool Python27ScriptingEngine::init( IDefinitionManager& definitionManager,
	IObjectManager& objectManager )
{
	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	// Disable importing Lib/site.py on startup.
	Py_NoSiteFlag = 1;
	// Enable debug output
	// Requires the ScriptOutputWriter output hook from stdout/stderr
	//Py_VerboseFlag = 2;
	// Use environment variables
	Py_IgnoreEnvironmentFlag = 0;

	// Initialize logging as a standard module
	// Must be before Py_Initialize()
	PyImport_AppendInittab( "ScriptOutputWriter",
		PyScript::PyInit_ScriptOutputWriter );

	Py_Initialize();
	
	// Import the logging module
	// Must be after Py_Initialize()
	PyImport_ImportModule( "ScriptOutputWriter" );

	ReflectionModule reflectedModule( definitionManager, objectManager );

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


void Python27ScriptingEngine::registerTypeConverter(
	IPythonTypeConverter & converter )
{
	impl_->typeConverters_.push_back( &converter );
}


void Python27ScriptingEngine::deregisterTypeConverter(
	IPythonTypeConverter & converter )
{
	auto found = std::find( impl_->typeConverters_.cbegin(),
		impl_->typeConverters_.cend(),
		&converter );
	if (found == impl_->typeConverters_.cend())
	{
		return;
	}
	impl_->typeConverters_.erase( found );
}


bool Python27ScriptingEngine::toScriptObject(
	const Variant & inVariant,
	PyScript::ScriptObject & outObject ) const
{
	for (auto itr = impl_->typeConverters_.crbegin();
		itr != impl_->typeConverters_.crend();
		++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert( pTypeConverter != nullptr );
		if (pTypeConverter->toScriptObject( inVariant, outObject ))
		{
			return true;
		}
	}

	for (auto itr = impl_->defaultTypeConverters_.crbegin();
		itr != impl_->defaultTypeConverters_.crend();
		++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert( pTypeConverter != nullptr );
		if (pTypeConverter->toScriptObject( inVariant, outObject ))
		{
			return true;
		}
	}
	return false;
}


bool Python27ScriptingEngine::toVariant(
	const PyScript::ScriptObject & inObject,
	Variant & outVariant ) const
{
	for (auto itr = impl_->typeConverters_.crbegin();
		itr != impl_->typeConverters_.crend();
		++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert( pTypeConverter != nullptr );
		if (pTypeConverter->toVariant( inObject, outVariant ))
		{
			return true;
		}
	}

	for (auto itr = impl_->defaultTypeConverters_.crbegin();
		itr != impl_->defaultTypeConverters_.crend();
		++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert( pTypeConverter != nullptr );
		if (pTypeConverter->toVariant( inObject, outVariant ))
		{
			return true;
		}
	}
	return false;
}

