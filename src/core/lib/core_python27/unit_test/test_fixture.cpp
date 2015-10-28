#include "pch.hpp"
#include "test_fixture.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"


TestFixture::TestFixture()
	: context_( nullptr )
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


TestFixture::~TestFixture()
{
	scriptingEngine_.fini( context_ );

	while (interfaces_.size())
	{
		context_.deregisterInterface( interfaces_.top() );
		interfaces_.pop();
	}
}


IComponentContext& TestFixture::componentContext()
{
	return context_;
}


IObjectManager& TestFixture::objectManager()
{
	return objectManager_;
}


IDefinitionManager& TestFixture::definitionManager()
{
	return definitionManager_;
}


IPythonScriptingEngine& TestFixture::scriptingEngine()
{
	return scriptingEngine_;
}
