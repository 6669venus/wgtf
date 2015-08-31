#include "pch.hpp"

#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_variant/variant.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"

TEST_F( TestDefinitionFixture, createGenericObject )
{
	auto handle = GenericObject::create( getDefinitionManager() );
	auto genericObj = handle.get();
	CHECK( genericObj != nullptr );

	int intValue = 1234;
	genericObj->set( "intValue", intValue );
	CHECK_EQUAL( intValue, genericObj->get< int >( "intValue" ) );

	int newIntValue = 5678;
	genericObj->set( "intValue", newIntValue );
	CHECK_EQUAL( newIntValue, genericObj->get< int >( "intValue" ) );

	Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );
	CHECK_EQUAL( vector3Value, genericObj->get< Vector3 >( "vector3Value" ) );

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );
	CHECK_EQUAL( stringValue, genericObj->get< std::string >( "stringValue" ) );

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	TestStructure2 value = genericObj->get< TestStructure2 >( "testStructure" );
	CHECK( testStructure == value );

	ObjectHandleT< TestPolyStructure > testPolyStruct(
		getDefinitionManager().create< TestPolyStructure >() );
	genericObj->set( "testPolyStruct", ObjectHandle( testPolyStruct ) );
	ObjectHandle provider = genericObj->get< ObjectHandle >( "testPolyStruct" );
	CHECK( testPolyStruct.get() == provider.getBase< TestPolyStructure >() );

	ObjectHandleT< TestDerivedPolyStructure >  testDerivedPolyStructure(
		getDefinitionManager().create< TestDerivedPolyStructure >() );
	genericObj->set( "testPolyStruct", ObjectHandle( testDerivedPolyStructure ) );
	provider = genericObj->get< ObjectHandle >( "testPolyStruct" );
	CHECK( testDerivedPolyStructure.get() == provider.getBase< TestDerivedPolyStructure >() );

	auto testDefinitionObject = 
		getDefinitionManager().create< TestDefinitionObject >();
	genericObj->set( "testDefinitionObject", testDefinitionObject );
	provider = genericObj->get< ObjectHandle >( "testDefinitionObject" );
	CHECK( testDefinitionObject.get() == provider.getBase< TestDefinitionObject >() );
}

TEST_F( TestDefinitionFixture, createMultipleGenericObjects )
{
	auto handle = GenericObject::create( getDefinitionManager() );
	auto genericObj = handle.get();
	CHECK( genericObj != nullptr );

	int intValue = 1234;
	genericObj->set( "intValue", intValue );

	Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );

	auto newHandle = handle.getDefinition( getDefinitionManager() )->createManagedObject();
	auto newGenericObj = newHandle.getBase< GenericObject >();

	CHECK( intValue == genericObj->get< int >( "intValue" ) );
	CHECK( vector3Value == genericObj->get< Vector3 >( "vector3Value" ) );
	CHECK( stringValue == genericObj->get< std::string >( "stringValue" ) );

	CHECK( intValue != newGenericObj->get< int >( "intValue" ) );
	CHECK( vector3Value != newGenericObj->get< Vector3 >( "vector3Value" ) );
	CHECK( stringValue != newGenericObj->get< std::string >( "stringValue" ) );

	int newIntValue = 5678;
	newGenericObj->set( "intValue", newIntValue );

	CHECK( intValue == genericObj->get< int >( "intValue" ) );
	CHECK( newIntValue == newGenericObj->get< int >( "intValue" ) );

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	TestStructure2 value = genericObj->get< TestStructure2 >( "testStructure" );
	CHECK( testStructure == value );
	TestStructure2 newValue = newGenericObj->get< TestStructure2 >( "testStructure" );
	CHECK( newValue == TestStructure2() );
}