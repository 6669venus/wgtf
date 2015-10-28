#include "pch.hpp"
#include "test_fixture.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_generic_plugin_manager/default_context_manager.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"
#include "core_logging/logging.hpp"

#include <stack>


struct TestFixture::Implementation
{
	Implementation( TestFixture& self )
		: self_( self )
		, context_( nullptr )
		, objectManager_()
		, definitionManager_( objectManager_ )
	{
		objectManager_.init( &definitionManager_ );
		Reflection::initReflectedTypes( definitionManager_ );

		IDefinitionManager& definitionManager = definitionManager_;
		REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
		REGISTER_DEFINITION( Scenario );

		scriptingEngine_.init( context_ );
		interfaces_.push( context_.registerInterface( &objectManager_, false ) );
		interfaces_.push( context_.registerInterface( &definitionManager_, false ) );
		interfaces_.push( context_.registerInterface( &scriptingEngine_, false ) );
	}


	~Implementation()
	{
		scriptingEngine_.fini( context_ );

		while (interfaces_.size())
		{
			context_.deregisterInterface( interfaces_.top() );
			interfaces_.pop();
		}
	}


	TestFixture& self_;
	DefaultComponentContext context_;
	ObjectManager objectManager_;
	DefinitionManager definitionManager_;
	Python27ScriptingEngine scriptingEngine_;
	std::stack<IInterface*> interfaces_;
};


TestFixture::TestFixture()
	: impl_( new Implementation( *this ) )
{
}


TestFixture::~TestFixture()
{
	impl_.reset();
}


IComponentContext& TestFixture::componentContext() const
{
	return impl_->context_;
}


IObjectManager& TestFixture::objectManager() const
{
	return impl_->objectManager_;
}


IDefinitionManager& TestFixture::definitionManager() const
{
	return impl_->definitionManager_;
}


IPythonScriptingEngine& TestFixture::scriptingEngine() const
{
	return impl_->scriptingEngine_;
}
