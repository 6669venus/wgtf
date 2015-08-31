#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/utilities/reflection_method_utilities.hpp"

#include "test_objects.hpp"


BEGIN_EXPOSE( TestDefinitionObject, ReflectedPolyStruct, MetaNone() )
	EXPOSE( "counter", counter_ )
	EXPOSE( "text", text_ )
	EXPOSE( "functional counter", getCounter, setCounter, MetaNone() )
	EXPOSE( "functional text", getText, setText, MetaNone() )
	EXPOSE( "counter getter", getCounter )
	EXPOSE( "text getter", getText )
	EXPOSE( "raw string", raw_string_ )
	EXPOSE( "string", string_ )
	EXPOSE( "strings", strings_ )
	EXPOSE( "raw wstring", raw_wstring_ )
	EXPOSE( "wstring", wstring_ )
	EXPOSE( "wstrings", wstrings_ )
	EXPOSE( "exposed structure", exposedStruct_ )
	EXPOSE( "exposed structures", exposedStructs_ )
	EXPOSE( "exposed polystructure", exposedPolyStruct_ )
	EXPOSE( "exposed polystructures", exposedPolyStructs_ )
	EXPOSE( "link", link_ )
	EXPOSE( "links", links_ )
	EXPOSE( "boolean", boolean_ )
	EXPOSE( "booleans", booleans_ )
	EXPOSE( "uint32", uint32_ )
	EXPOSE( "uint32s", uint32s_ )
	EXPOSE( "int32", int32_ )
	EXPOSE( "int32s", int32s_ )
	EXPOSE( "uint64", uint64_ )
	EXPOSE( "uint64s", uint64s_ )
	EXPOSE( "float", float_ )
	EXPOSE( "floats", floats_ )
	EXPOSE( "vector3", vector3_ )
	EXPOSE( "vector3s", vector3s_ )
	EXPOSE( "vector4", vector4_ )
	EXPOSE( "vector4s", vector4s_ )
	EXPOSE( "binary", binary_ )
	EXPOSE( "binaries", binaries_ )
END_EXPOSE()

BEGIN_EXPOSE( TestDefinitionDerivedObject, TestDefinitionObject, MetaNone() )
	EXPOSE( "some integer", someInteger_ )
	EXPOSE( "some float", someFloat_ )
END_EXPOSE()

BEGIN_EXPOSE( TestStructure2, MetaNone() )
	EXPOSE( "name", name_ )
END_EXPOSE()

BEGIN_EXPOSE( TestPolyStructure, ReflectedPolyStruct, MetaNone() )
	EXPOSE( "enabled", getEnabled, setEnabled, MetaNone() )
END_EXPOSE()

BEGIN_EXPOSE( TestDerivedPolyStructure, TestPolyStructure, MetaNone() )
	EXPOSE( "length", length_ )
END_EXPOSE()

TestDefinitionFixture::TestDefinitionFixture()
{
	IDefinitionManager & definitionManager = getDefinitionManager();
	REGISTER_DEFINITION( TestStructure2 );
	REGISTER_DEFINITION( TestPolyStructure );
	REGISTER_DEFINITION( TestDerivedPolyStructure );
	REGISTER_DEFINITION( TestDefinitionObject );
	REGISTER_DEFINITION( TestDefinitionDerivedObject );
	klass_ = definitionManager.getDefinition< TestDefinitionObject >();
	derived_klass_ = definitionManager.getDefinition< TestDefinitionDerivedObject >();
}

namespace
{
	uint32_t RandomNumber32() 
	{
		RefObjectId uid( RefObjectId::generate() );
		return uid.getA() + uid.getB() + uid.getC() + uid.getD();
	}

	uint64_t RandomNumber64() 
	{
		RefObjectId uid( RefObjectId::generate() );
		uint64_t a = ( uint64_t ) uid.getA();
		uint64_t b = ( uint64_t ) uid.getB();
		uint64_t c = ( uint64_t ) uid.getC();
		uint64_t d = ( uint64_t ) uid.getD();
		return (a << 32) + (b << 32) + c + d;
	}

	std::string RandomString() 
	{
		std::string random( "Random Data: " );
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		return random;
	}
}


//------------------------------------------------------------------------------
TestDefinitionObject::TestDefinitionObject() :
	counter_( 0 ), raw_string_( "RAW READ-0NLY STR1NG!!!" ), 
	raw_wstring_( L"RAW READ-0NLY W1DE STR1NG!!!" ), boolean_( false ),
	uint32_( 0 ), int32_( 0 ), uint64_( 0L ), float_( 0.f )
{
}


//------------------------------------------------------------------------------
void TestDefinitionObject::initialise( int value, ObjectHandleT< ReflectedPolyStruct > anyObj )
{
	counter_ = value;
	
	std::stringstream ss;
	ss << "TestDefinitionObject " << value + 1;
	text_ = ss.str();

	string_ = RefObjectId::generate().toString();
	strings_.push_back( RefObjectId::generate().toString() );
	strings_.push_back( RefObjectId::generate().toString() );

	std::wstringstream wss;
	wss << L"TestDefinitionObject " << value + 1 << L" (ÔÓ-ÛÒÒÍË)";
	wstring_ = wss.str();
	wstrings_.push_back( wstring_ );
	wstrings_.push_back( wstring_ );
	
	auto guid = RefObjectId::generate();
	exposedStruct_.name_ = guid.toString();
	TestStructure2 testStructure;
	guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	exposedStructs_.push_back( testStructure );
	guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	exposedStructs_.push_back( testStructure );

	exposedPolyStruct_ = 
		getDefinition().getDefinitionManager()->create< TestPolyStructure >();
	exposedPolyStruct_->enabled_ = value % 2 == 0;

	ObjectHandleT< TestPolyStructure > polyStruct(
		getDefinition().getDefinitionManager()->create< TestPolyStructure >() );
	polyStruct->enabled_ = value % 2 != 0;
	exposedPolyStructs_.push_back( polyStruct );

	ObjectHandleT< TestDerivedPolyStructure > derivedPolyStruct =
		getDefinition().getDefinitionManager()->create< TestDerivedPolyStructure >();
	derivedPolyStruct->enabled_ = value % 2 == 0;
	derivedPolyStruct->length_ = (float) value / 10.f;
	exposedPolyStructs_.push_back( derivedPolyStruct );

	ObjectHandleT< ReflectedPolyStruct > link_ = anyObj;
	links_.push_back( link_ );

	boolean_ = value % 2 == 0;
	booleans_.push_back( value % 2 == 0 );
	booleans_.push_back( value % 2 != 0 );

	uint32_ = value;
	uint32s_.resize( 5 );
	std::generate( uint32s_.begin(), uint32s_.end(), RandomNumber32 );

	int32_ = value;
	int32s_.resize(  5);
	std::generate( int32s_.begin(), int32s_.end(), RandomNumber32 );

	uint64_ = value;
	uint64s_.resize( 5 );
	std::generate( uint64s_.begin(), uint64s_.end(), RandomNumber64 );

	float_ = (float) value;
	floats_.resize( 5 );
	std::generate( floats_.begin(), floats_.end(), RandomNumber32 );

	vector3_ = Vector3( value + 1.f, value + 2.f, value + 100.f );
	vector3s_.push_back( Vector3( value + 10.f, value + 20.f, value + 200.f ) );
	vector3s_.push_back( Vector3( value + 100.f, value +200.f, value + 300.f ) );

	vector4_ = Vector4( value + 2.f, value + 4.f, value + 8.f, value - 100.f );
	vector4s_.push_back( Vector4( value + 16.f, value + 32.f, value + 64.f, value - 200.f ) );
	vector4s_.push_back( Vector4( value + 256.f, value + 512.f, value + 1024.f, value - 300.f ) );
	
	std::string randomData( RandomString() );
	binary_ = std::make_shared< BinaryBlock >(
		randomData.c_str(), randomData.size(), false );
	binaries_.push_back( std::make_shared< BinaryBlock >(
		randomData.c_str(), randomData.size(), false  ) );
	std::string moreRandomData( RandomString() );
	binaries_.push_back( std::make_shared< BinaryBlock >(
		moreRandomData.c_str(), moreRandomData.size(), false ) );
}


//------------------------------------------------------------------------------
bool TestDefinitionObject::operator==( const TestDefinitionObject& tdo ) const
{
	if (counter_ != tdo.counter_)
		return false;

	if (text_ != tdo.text_)
		return false;

	if (std::string(raw_string_) != std::string( tdo.raw_string_ ))
		return false;

	if (string_ != tdo.string_  || strings_ != tdo.strings_)
		return false;

	if (std::wstring( raw_wstring_ ) != std::wstring( tdo.raw_wstring_ ))
		return false;

	if (wstring_ != tdo.wstring_ || wstrings_ != tdo.wstrings_)
		return false;

	if (exposedStruct_ != tdo.exposedStruct_ || exposedStructs_ != tdo.exposedStructs_)
		return false;

	if (*exposedPolyStruct_  != *tdo.exposedPolyStruct_  ||
		exposedPolyStructs_.size() != tdo.exposedPolyStructs_.size())
		return false;

	auto i = 0u;
	for (; i < exposedPolyStructs_.size() && 
		*exposedPolyStructs_[i].get() == *tdo.exposedPolyStructs_[i].get(); ++i);
		if (i != exposedPolyStructs_.size())
			return false;

	if (!(link_.get() == tdo.link_.get() && links_ == tdo.links_))
		return false;

	if (boolean_ != tdo.boolean_ || booleans_ != tdo.booleans_)
		return false;

	if (uint32_ != tdo.uint32_ || uint32s_ != tdo.uint32s_)
		return false;

	if (int32_ != tdo.int32_ || int32s_ != tdo.int32s_)
		return false;

	if (uint64_ != tdo.uint64_ || uint64s_ != tdo.uint64s_)
		return false;

	if (float_ != tdo.float_ || floats_ != tdo.floats_)
		return false;

	if (vector3_ != tdo.vector3_ || vector3s_ != tdo.vector3s_)
		return false;

	if (vector4_ != tdo.vector4_ || vector4s_ != tdo.vector4s_)
		return false;

	if (binary_->compare( *tdo.binary_ ) != 0 || binaries_.size() != tdo.binaries_.size())
		return false;

	for (i = 0; i < binaries_.size() && 
		binaries_[i]->compare( *tdo.binaries_[i] ) == 0; ++i);
		if (i != binaries_.size())
			return false;

	return true;
}


//------------------------------------------------------------------------------
bool TestDefinitionObject::operator!=( const TestDefinitionObject & tdo ) const
{
	return !operator==( tdo );
}
