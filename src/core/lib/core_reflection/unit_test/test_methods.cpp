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
	ReflectedMethodParameters parameters;

	auto pa = klass_->bindProperty( "TestMethod1", object );
	CHECK( pa.isValid() );
	Variant result = pa.invoke( parameters );

	pa = klass_->bindProperty( "TestMethod2", object );
	CHECK( pa.isValid() );
	result = pa.invoke( parameters );

	pa = klass_->bindProperty( "TestMethod3", object );
	CHECK( pa.isValid() );
	parameters.push_back( int( 5 ) );
	result = pa.invoke( parameters );
	std::string testResult;
	result.tryCast( testResult );
	CHECK( testResult == "test3" );

	pa = klass_->bindProperty( "TestMethod4", object );
	CHECK( pa.isValid() );
	std::string parameterString = "test";
	ObjectHandle parameter1 = parameterString;
	parameters = Variant( parameter1 );
	parameters.push_back( 5 );
	result = pa.invoke( parameters );
	ObjectHandle checkHandle = parameters[0].cast<ObjectHandle>();
	testResult = *checkHandle.getBase<std::string>();
	CHECK( testResult == "test" );
	CHECK( parameterString == "test" );

	pa = klass_->bindProperty( "TestMethod5", object );
	CHECK( pa.isValid() );
	parameters = Variant( parameter1 );
	result = pa.invoke( parameters );
	checkHandle = parameters[0].cast<ObjectHandle>();
	testResult = *checkHandle.getBase<std::string>();
	CHECK( testResult == "test5" );
	CHECK( parameterString == "test5" );
}
