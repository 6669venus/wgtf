#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_python27/script_object_definition_registry.hpp"

#include "core_python27/type_converters/bool_converter.hpp"
#include "core_python27/type_converters/dict_converter.hpp"
#include "core_python27/type_converters/double_converter.hpp"
#include "core_python27/type_converters/int_converter.hpp"
#include "core_python27/type_converters/list_converter.hpp"
#include "core_python27/type_converters/long_converter.hpp"
#include "core_python27/type_converters/string_converter.hpp"
#include "core_python27/type_converters/tuple_converter.hpp"
#include "core_python27/type_converters/type_converter.hpp"
#include "core_python27/type_converters/unicode_converter.hpp"

#include <stack>


/**
 *	Controls initialization and finalization of Python and
 *	registers the Python interface to be used by other plugins.
 */
class Python27Plugin
	: public PluginMain
{
public:
	Python27Plugin( IComponentContext & contextManager )
		: interpreter_( contextManager )
		, definitionRegistry_( contextManager )
		, listTypeConverter_( typeConverters_ )
		, tupleTypeConverter_( typeConverters_ )
		, dictTypeConverter_( typeConverters_ )
		, defaultTypeConverter_( contextManager )
		, pTypeConvertersInterface_( nullptr )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		const bool transferOwnership = false;
		interfaces_.push( contextManager.registerInterface( &interpreter_, transferOwnership ) );
		interfaces_.push( contextManager.registerInterface( &definitionRegistry_, transferOwnership ) );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		auto pDefinitionManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager_ == nullptr)
		{
			return;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager_);
		REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
		REGISTER_DEFINITION( Scenario );

		interpreter_.init();

		// Register type converters for converting between PyObjects and Variant
		typeConverters_.registerTypeConverter( defaultTypeConverter_ );
		typeConverters_.registerTypeConverter( stringTypeConverter_ );
		typeConverters_.registerTypeConverter( unicodeTypeConverter_ );
		typeConverters_.registerTypeConverter( listTypeConverter_ );
		typeConverters_.registerTypeConverter( tupleTypeConverter_ );
		typeConverters_.registerTypeConverter( dictTypeConverter_ );
		typeConverters_.registerTypeConverter( boolTypeConverter_ );
		typeConverters_.registerTypeConverter( intTypeConverter_ );
		typeConverters_.registerTypeConverter( longTypeConverter_ );
		typeConverters_.registerTypeConverter( doubleTypeConverter_ );
		const bool transferOwnership = false;
		pTypeConvertersInterface_ = contextManager.registerInterface(
			&typeConverters_,
			transferOwnership,
			IComponentContext::Reg_Local );
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		// Deregister type converters for converting between PyObjects and Variant
		typeConverters_.deregisterTypeConverter( doubleTypeConverter_ );
		typeConverters_.deregisterTypeConverter( longTypeConverter_ );
		typeConverters_.deregisterTypeConverter( intTypeConverter_ );
		typeConverters_.deregisterTypeConverter( boolTypeConverter_ );
		typeConverters_.deregisterTypeConverter( tupleTypeConverter_ );
		typeConverters_.deregisterTypeConverter( dictTypeConverter_ );
		typeConverters_.deregisterTypeConverter( listTypeConverter_ );
		typeConverters_.deregisterTypeConverter( unicodeTypeConverter_ );
		typeConverters_.deregisterTypeConverter( stringTypeConverter_ );
		typeConverters_.deregisterTypeConverter( defaultTypeConverter_ );
		contextManager.deregisterInterface( pTypeConvertersInterface_ );

		interpreter_.fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
		while (!interfaces_.empty())
		{
			contextManager.deregisterInterface( interfaces_.top() );
			interfaces_.pop();
		}
	}

private:
	std::stack<IInterface*> interfaces_;
	Python27ScriptingEngine interpreter_;
	ScriptObjectDefinitionRegistry definitionRegistry_;

	PythonTypeConverters typeConverters_;

	PythonType::StringConverter stringTypeConverter_;
	PythonType::UnicodeConverter unicodeTypeConverter_;
	PythonType::ListConverter listTypeConverter_;
	PythonType::TupleConverter tupleTypeConverter_;
	PythonType::DictConverter dictTypeConverter_;
	PythonType::TypeConverter defaultTypeConverter_;
	PythonType::DoubleConverter doubleTypeConverter_;
	PythonType::LongConverter longTypeConverter_;
	PythonType::IntConverter intTypeConverter_;
	PythonType::BoolConverter boolTypeConverter_;

	IInterface * pTypeConvertersInterface_;
};

PLG_CALLBACK_FUNC( Python27Plugin )

