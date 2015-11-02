#include "pch.hpp"
#include "test_python_fixture.hpp"
#include "reflection_test_module.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_variant/variant.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"


TestPythonFixture::TestPythonFixture( const char * testName,
	TestResult & result )
{
	auto & context = *pluginManager_.getContextManager().getGlobalContext();

	Variant::setMetaTypeManager(
		context.queryInterface< IMetaTypeManager >() );

	IDefinitionManager * pDefinitionManager =
			context.queryInterface< IDefinitionManager >();
	assert( pDefinitionManager != nullptr );
	auto & definitionManager = (*pDefinitionManager);
	REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
	REGISTER_DEFINITION( Scenario );

	scriptingEngine_.init( context );

	reflectionModule_.reset( new ReflectionTestModule( context,
		testName,
		result ) );

	interfaces_.push( context.registerInterface( &scriptingEngine_, false ) );
}


TestPythonFixture::~TestPythonFixture()
{
	auto & context = *pluginManager_.getContextManager().getGlobalContext();

	// Module is de-registered by Py_Finalize
	reflectionModule_.reset( nullptr );

	scriptingEngine_.fini( context );

	while (interfaces_.size())
	{
		context.deregisterInterface( interfaces_.top() );
		interfaces_.pop();
	}
}

