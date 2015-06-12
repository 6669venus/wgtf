#include "pch.hpp"

#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/reflected_property.hpp"
#include "reflection/function_property.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/definition_manager.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"

#include "wg_types/binary_block.hpp"

#include "test_objects.hpp"


BEGIN_EXPOSE( TestCommandObject, MetaNone() )
	EXPOSE( "counter", counter_ )
	EXPOSE( "text", text_ )
	EXPOSE( "functional counter", getCounter, setCounter, MetaNone() )
	EXPOSE( "functional text", getText, setText, MetaNone() )
	EXPOSE( "string", string_ )
	EXPOSE( "strings", strings_ )
	EXPOSE( "wstring", wstring_ )
	EXPOSE( "wstrings", wstrings_ )
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
	EXPOSE( "binary", binary_ )
	EXPOSE( "binaries", binaries_ )
END_EXPOSE()



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
TestCommandObject::TestCommandObject() :
	counter_( 0 ), boolean_( false ),
	uint32_( 0 ), int32_( 0 ), uint64_( 0L ), float_( 0.f )
{
}


//------------------------------------------------------------------------------
void TestCommandObject::initialise( int value )
{
	counter_ = value;
	
	std::stringstream ss;
	ss << "TestDefinitionObject " << value + 1;
	text_ = ss.str();

	string_ = RefObjectId::generate().toString();
	strings_.push_back( RefObjectId::generate().toString() );
	strings_.push_back( RefObjectId::generate().toString() );

	std::wstringstream wss;
	wss << L"TestDefinitionObject " << value + 1 << L" (по-русски)";
	wstring_ = wss.str();
	wstrings_.push_back( wstring_ );
	wstrings_.push_back( wstring_ );

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
bool TestCommandObject::operator==( const TestCommandObject& tdo ) const
{
	if (counter_ != tdo.counter_)
		return false;

	if (text_ != tdo.text_)
		return false;

	if (string_ != tdo.string_  || strings_ != tdo.strings_)
		return false;

	if (wstring_ != tdo.wstring_ || wstrings_ != tdo.wstrings_)
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

	if (binary_->compare( *tdo.binary_ ) != 0 || binaries_.size() != tdo.binaries_.size())
		return false;

	int i = 0;
	for (i = 0; i < binaries_.size() && 
		binaries_[i]->compare( *tdo.binaries_[i] ) == 0; ++i);
		if (i != binaries_.size())
			return false;

	return true;
}


//------------------------------------------------------------------------------
bool TestCommandObject::operator!=( const TestCommandObject & tdo ) const
{
	return !operator==( tdo );
}

