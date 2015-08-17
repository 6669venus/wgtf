#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"


TEST_F( TestMethodsFixture, methods )
{
	ObjectHandle object = klass_->createManagedObject();
	PropertyIterator itr = klass_->getPropertyIterator();
	IBaseProperty* method = findProperty( itr, "TestMethod1" );
	assert( method );

	ReflectedMethodParameters parameters;
	Variant result = method->invoke( object, parameters );

	method = findProperty( itr, "TestMethod2" );
	CHECK( method );
	result = method->invoke( object, parameters );

	method = findProperty( itr, "TestMethod3" );
	CHECK( method );
	parameters.push_back( int( 5 ) );
	result = method->invoke( object, parameters );
	std::string testResult;
	result.tryCast( testResult );
	CHECK( testResult == "test3" );

	method = findProperty( itr, "TestMethod4" );
	CHECK( method );
	std::string parameterString = "test";
	ObjectHandle parameter1 = parameterString;
	parameters = parameter1;
	parameters.push_back( 5 );
	result = method->invoke( object, parameters );
	testResult = *parameters[0].getBase<std::string>();
	CHECK( testResult == "test" );
	CHECK( parameterString == "test" );

	method = findProperty( itr, "TestMethod5" );
	CHECK( method );
	parameters = parameter1;
	result = method->invoke( object, parameters );
	testResult = *parameters[0].getBase<std::string>();
	CHECK( testResult == "test5" );
	CHECK( parameterString == "test5" );
}
