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
	CHECK_EQUAL( Variant( intValue ), genericObj->get( "intValue" ) );

	int newIntValue = 5678;
	genericObj->set( "intValue", newIntValue );
	CHECK_EQUAL( Variant( newIntValue ), genericObj->get( "intValue" ) );

	BW::Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );
	CHECK_EQUAL( Variant( vector3Value ), genericObj->get( "vector3Value" ) );

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );
	CHECK_EQUAL( Variant( stringValue ), genericObj->get( "stringValue" ) );

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	Variant structVariant = genericObj->get( "testStructure" );
	TestStructure2 value;
	structVariant.tryCast( value );
	CHECK( testStructure == value );

	ObjectHandleT< TestPolyStructure > testPolyStruct(
		getDefinitionManager().create< TestPolyStructure >() );
	genericObj->set( "testPolyStruct", ObjectHandle( testPolyStruct ) );
	structVariant = genericObj->get( "testPolyStruct" );
	ObjectHandle provider;
	structVariant.tryCast( provider );
	CHECK( testPolyStruct.get() == provider.getBase< TestPolyStructure >() );

	ObjectHandleT< TestDerivedPolyStructure >  testDerivedPolyStructure(
		getDefinitionManager().create< TestDerivedPolyStructure >() );
	genericObj->set( "testPolyStruct", ObjectHandle( testDerivedPolyStructure ) );
	structVariant = genericObj->get( "testPolyStruct" );
	structVariant.tryCast( provider );
	CHECK( testDerivedPolyStructure.get() == provider.getBase< TestDerivedPolyStructure >() );

	auto testDefinitionObject = 
		getDefinitionManager().createT< TestDefinitionObject >();
	genericObj->set( "testDefinitionObject", testDefinitionObject );
	structVariant = genericObj->get( "testDefinitionObject" );
	structVariant.tryCast( provider );
	CHECK( testDefinitionObject.get() == provider.getBase< TestDefinitionObject >() );
}

TEST_F( TestDefinitionFixture, createMultipleGenericObjects )
{
	auto handle = GenericObject::create( getDefinitionManager() );
	auto genericObj = handle.get();
	CHECK( genericObj != nullptr );

	int intValue = 1234;
	genericObj->set( "intValue", intValue );

	BW::Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );

	auto newHandle = handle.getDefinition()->createManagedObject();
	auto newGenericObj = newHandle.getBase< GenericObject >();

	CHECK( Variant( intValue ) == genericObj->get( "intValue" ) );
	CHECK( Variant( vector3Value ) == genericObj->get( "vector3Value" ) );
	CHECK( Variant( stringValue ) == genericObj->get( "stringValue" ) );

	CHECK( Variant( intValue ) != newGenericObj->get( "intValue" ) );
	CHECK( Variant( vector3Value ) != newGenericObj->get( "vector3Value" ) );
	CHECK( Variant( stringValue ) != newGenericObj->get( "stringValue" ) );

	int newIntValue = 5678;
	newGenericObj->set( "intValue", newIntValue );

	CHECK( Variant( intValue ) == genericObj->get( "intValue" ) );
	CHECK( Variant( newIntValue ) == newGenericObj->get( "intValue" ) );

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	Variant structVariant = genericObj->get( "testStructure" );
	TestStructure2 value;
	structVariant.tryCast( value );
	CHECK( testStructure == value );
	structVariant = newGenericObj->get( "testStructure" );
	CHECK( !structVariant.tryCast( value ) );
}