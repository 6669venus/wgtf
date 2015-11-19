#include "pch.hpp"
#include "definition_details.hpp"

#include "core_logging/logging.hpp"

#include "scripting_engine.hpp"
#include "defined_instance.hpp"
#include "type_converters/python_meta_type.hpp"

#include "core_variant/interfaces/i_meta_type_manager.hpp"
#include "core_variant/variant.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/object_handle.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"


#include <array>
#include <vector>
#include <map>
#include <mutex>


struct Python27ScriptingEngine::Implementation
{
	Implementation( Python27ScriptingEngine& self, IComponentContext& context );

	void initialiseTypeConverters();
	void finaliseTypeConverters();

	IClassDefinition* registerObject( const PyScript::ScriptObject& object );
	void deregisterObject( const PyScript::ScriptObject& object );

	Python27ScriptingEngine& self_;
	IComponentContext& context_;

	std::map<PyScript::ScriptObject, std::pair<size_t, IClassDefinition*>> definitions_;
	std::mutex definitionsMutex_;

	std::vector<std::unique_ptr<MetaType>> defaultMetaTypes_;

	PythonTypeConverters typeConverters_;
	PythonType::StringConverter defaultTypeConverter_;
	PythonType::ListConverter listTypeConverter_;
	PythonType::TupleConverter tupleTypeConverter_;
	PythonType::TypeConverter typeTypeConverter_;
	PythonType::LongConverter longTypeConverter_;
	IInterface* pTypeConvertersInterface_;
};


Python27ScriptingEngine::Implementation::Implementation( Python27ScriptingEngine& self, IComponentContext& context )
	: self_( self )
	, context_( context )
	, listTypeConverter_( typeConverters_ )
	, tupleTypeConverter_( typeConverters_ )
	, pTypeConvertersInterface_( nullptr )
{
}


void Python27ScriptingEngine::Implementation::initialiseTypeConverters()
{
	// Register Python types to be usable by Variant
	auto pMetaTypeManager = context_.queryInterface< IMetaTypeManager >();
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
	typeConverters_.registerTypeConverter( listTypeConverter_ );
	typeConverters_.registerTypeConverter( tupleTypeConverter_ );
	typeConverters_.registerTypeConverter( typeTypeConverter_ );
	typeConverters_.registerTypeConverter( longTypeConverter_ );

	const bool transferOwnership = false;
	pTypeConvertersInterface_ = context_.registerInterface(
		&typeConverters_, transferOwnership, IComponentContext::Reg_Local );
}


void Python27ScriptingEngine::Implementation::finaliseTypeConverters()
{
	// Deregister type converters for converting between PyObjects and Variant
	typeConverters_.deregisterTypeConverter( longTypeConverter_ );
	typeConverters_.deregisterTypeConverter( typeTypeConverter_ );
	typeConverters_.deregisterTypeConverter( tupleTypeConverter_ );
	typeConverters_.deregisterTypeConverter( listTypeConverter_ );
	typeConverters_.deregisterTypeConverter( defaultTypeConverter_ );
	context_.deregisterInterface( pTypeConvertersInterface_ );

	// Register Python types to be usable by Variant
	auto pMetaTypeManager = context_.queryInterface< IMetaTypeManager >();
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
}


IClassDefinition* Python27ScriptingEngine::Implementation::registerObject( const PyScript::ScriptObject& object )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	auto itr = definitions_.find( object );

	if (itr != definitions_.end())
	{
		++itr->second.first;
		return itr->second.second;
	}
	
	auto definitionManager = context_.queryInterface<IDefinitionManager>();
	assert( definitionManager != nullptr );

	auto& definitionReference = definitions_[object];
	definitionReference.first = 1;
	definitionReference.second =
		definitionManager->registerDefinition( new ReflectedPython::DefinitionDetails( context_, object ) );
	assert( definitionReference.second != nullptr );

	return definitionReference.second;
}


void Python27ScriptingEngine::Implementation::deregisterObject( const PyScript::ScriptObject& object )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	auto itr = definitions_.find( object );
	assert( itr != definitions_.end() );

	if (itr == definitions_.end())
	{
		return;
	}

	auto& definitionReference = itr->second;
	--definitionReference.first;
	
	if (definitionReference.first != 0)
	{
		return;
	}

	assert( definitionReference.second != nullptr );
	IDefinitionManager* definitionManager = definitionReference.second->getDefinitionManager();

	if (definitionManager != nullptr)
	{
		definitionManager->deregisterDefinition( definitionReference.second );
	}

	definitions_.erase( itr );
}


Python27ScriptingEngine::Python27ScriptingEngine( IComponentContext& context )
	: impl_( new Implementation( *this, context ) )
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

	impl_->initialiseTypeConverters();
	return true;
}


void Python27ScriptingEngine::fini()
{
	impl_->finaliseTypeConverters();

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
		new ReflectedPython::DefinedInstance( impl_->context_, module ) );
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


IClassDefinition* Python27ScriptingEngine::registerObject( const PyScript::ScriptObject& object )
{
	return impl_->registerObject( object );
}


void Python27ScriptingEngine::deregisterObject( const PyScript::ScriptObject& object )
{
	impl_->deregisterObject( object );
}
