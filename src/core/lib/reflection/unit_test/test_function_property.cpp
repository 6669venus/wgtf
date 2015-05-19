#include "pch.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "test_helpers.hpp"
#include "reflection/function_property.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/definition_manager.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "wg_types/binary_block.hpp"
#include "unit_test_lib/unit_test.hpp"
#include "test_reflection_fixture.hpp"

#include <memory>


// =============================================================================

class TestPropertyFixtureBase
	: public TestReflectionFixture
{
private:
	static TestPropertyFixtureBase * s_instance;

public:
	TestPropertyFixtureBase();

	~TestPropertyFixtureBase()
	{
		assert( s_instance != nullptr );
		s_instance = nullptr;
	}

	static TestPropertyFixtureBase & getFixture()
	{
		assert( s_instance != nullptr );
		return *s_instance;
	}

	typedef std::unique_ptr< BaseProperty > BasePropertyPtr;

	BasePropertyPtr booleanProperty_;
	BasePropertyPtr integerProperty_;
	BasePropertyPtr uintegerProperty_;
	BasePropertyPtr floatProperty_;
	BasePropertyPtr stringProperty_;
	BasePropertyPtr wstringProperty_;
	//BasePropertyPtr rawStringProperty_;
	//BasePropertyPtr rawWStringProperty_;
	BasePropertyPtr vector3Property_;
	BasePropertyPtr vector4Property_;
	BasePropertyPtr binaryDataProperty_;
	BasePropertyPtr exposedStructProperty_;
	BasePropertyPtr exposedPolyStructProperty_;
	BasePropertyPtr linkProperty_;

	class TestPropertyObject
	{
	public:
		bool boolean_;
		int integer_;
		unsigned int uinteger_;
		float floating_;
		BW::string string_;
		BW::wstring wstring_;
		const char * raw_string_;
		const wchar_t* raw_wstring_;
		BW::Vector3 vector3_;
		BW::Vector4 vector4_;
		std::shared_ptr< BinaryBlock > binary_data_;
		struct ExposedStruct
		{
			ExposedStruct() : boolean_( false ) {}
			bool boolean_;

			bool operator==( const ExposedStruct& other ) const
			{
				return other.boolean_ == boolean_;
			}
			bool operator!=( const ExposedStruct& other ) const
			{
				return !operator==( other );
			}
		};
		ExposedStruct exposedStruct_;
		struct ExposedPolyStruct : public ReflectedPolyStruct
		{
			DECLARE_REFLECTED
		public:
			ExposedPolyStruct()
				: ReflectedPolyStruct()
				, string_( "ExposedPolyStruct" ) {}
			BW::string string_;
		};
		typedef ObjectHandleT< ExposedPolyStruct > ExposedPolyStructPtr;
		ExposedPolyStructPtr exposedPolyStruct_;
		ObjectHandleT< ReflectedPolyStruct > link_;

		TestPropertyObject() :
			boolean_(false),
			integer_(0),
			uinteger_(0U),
			floating_(0.0f),
			string_(),
			wstring_(),
			raw_string_(NULL),
			raw_wstring_(NULL),
			vector3_(0.0f, 0.0f, 0.0f),
			vector4_(0.0f, 0.0f, 0.0f, 0.0f),
			binary_data_(),
			exposedStruct_(),
			exposedPolyStruct_(
				TestPropertyFixtureBase::getFixture().getDefinitionManager().create< ExposedPolyStruct >() )
		{
		}

#define IMPLEMENT_XETERS(Name, Type, Variable) \
		void set ## Name( const Type & value ) { Variable = value; } \
		Type get ## Name ## Value() const { return Variable; } \
		const Type & get ## Name ## Ref() const { return Variable; } \
		void get ## Name ## Arg( Type * value ) const { *value = Variable; }

		IMPLEMENT_XETERS(Boolean, bool, boolean_)
		IMPLEMENT_XETERS(Integer, int, integer_)
		IMPLEMENT_XETERS(UInteger, unsigned int, uinteger_)
		IMPLEMENT_XETERS(Float, float, floating_)
		IMPLEMENT_XETERS(String, BW::string, string_)
		IMPLEMENT_XETERS(WString, BW::wstring, wstring_)
		IMPLEMENT_XETERS(Vector3, BW::Vector3, vector3_)
		IMPLEMENT_XETERS(Vector4, BW::Vector4, vector4_)
		IMPLEMENT_XETERS(BinaryData, std::shared_ptr< BinaryBlock >, binary_data_)
		IMPLEMENT_XETERS( ExposedStruct,
			ExposedStruct,
			exposedStruct_ )
		IMPLEMENT_XETERS( ExposedPolyStruct,
			ExposedPolyStructPtr,
			exposedPolyStruct_ )
		IMPLEMENT_XETERS( Link,
			ObjectHandleT< ReflectedPolyStruct >,
			link_ )

#undef IMPLEMENT_XETERS


		// Raw string stuff is sketchy at the moment, highlighting some deficiencies in
		// the design of the reflection system.

		//void setRawString( const char * & value ) { raw_string_ = value; }
		//void getRawStringValue( const char * * value ) const { *value = raw_string_; }
		//const char * & getRawStringRef() const { return raw_string_; }

		//void setRawWString( const wchar_t * & value ) { raw_wstring_ = value; }
		//void getRawWStringValue( const wchar_t * * value ) const { *value = raw_wstring_; }
		//const wchar_t * & getRawWStringRef() const { return raw_wstring_; }
	};

	template <typename TargetType >
	bool setProperty( BaseProperty* property, ObjectHandle & pBase, const TargetType & value )
	{
		FunctionPropertySet<TargetType, TestPropertyObject> * propertySet = 
			dynamic_cast< FunctionPropertySet<TargetType, TestPropertyObject> * >( property );
		if (property == nullptr)
		{
			return false;
		}
		propertySet->set(pBase, value);
		return true;
	}
};

typedef TestPropertyFixtureBase::TestPropertyObject::ExposedStruct TestExposedStruct;
typedef TestPropertyFixtureBase::TestPropertyObject::ExposedPolyStruct TestExposedPolyStruct;

BEGIN_EXPOSE( TestExposedStruct, MetaNone() )
END_EXPOSE()

BEGIN_EXPOSE( TestExposedPolyStruct, ReflectedPolyStruct, MetaNone() )
END_EXPOSE()

TestPropertyFixtureBase * TestPropertyFixtureBase::s_instance = nullptr;

TestPropertyFixtureBase::TestPropertyFixtureBase()
{
	assert( s_instance == nullptr );

	IDefinitionManager & definitionManager = getDefinitionManager();
	REGISTER_DEFINITION( TestExposedStruct );
	REGISTER_DEFINITION( TestExposedPolyStruct );
	s_instance = this;
}

#define IMPLEMENT_TEST_F( FIXTURE, TEST )		\
	TEST_F( FIXTURE, TEST ) {					\
		test_##TEST( this, m_name, result_ );	\
	}

// To add a new test create a new X entry - X( new_test )
// and define the test function -
// template <typename FIXTURE>
// void test_new_test( FIXTURE* fixture, const char * m_name, TestResult& result_ ) {}
#define TESTS									\
	X( boolean_property )						\
	X( integer_property )						\
	X( unsigned_integer_property )				\
	X( float_property )							\
	X( string_property )						\
	X( wstring_property )						\
	X( vector3_property	)						\
	X( vector4_property )						\
	X( binary_data_property )					\
	X( exposed_struct_property )				\
	X( exposed_poly_struct_property )			\
	X( link_property )

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_boolean_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.boolean_ = false;

		bool value;
		Variant variant = fixture->booleanProperty_->get(provider );
		variant.tryCast( value );

		CHECK_EQUAL(false, subject_.boolean_);
		CHECK_EQUAL(subject_.boolean_, value);
	}

	{
		bool value = true;
		CHECK( fixture->setProperty<bool>( 
			fixture->booleanProperty_.get(), provider, value ) );
		CHECK_EQUAL(true, subject_.boolean_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_integer_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.integer_ = -3567345;

		int value;
		Variant variant = fixture->integerProperty_->get(provider);
		variant.tryCast( value );
		CHECK_EQUAL(-3567345, subject_.integer_);
		CHECK_EQUAL(subject_.integer_, value);
	}

	{
		int value = 5645654;
		CHECK( fixture->setProperty<int>( 
			fixture->integerProperty_.get(), provider, value ) );
		CHECK_EQUAL(5645654, subject_.integer_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_unsigned_integer_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.uinteger_ = 1321491649u;

		unsigned int value;
		Variant variant =
			fixture->uintegerProperty_->get(provider);
		variant.tryCast( value );

		CHECK_EQUAL(1321491649u, subject_.uinteger_);
		CHECK_EQUAL(subject_.uinteger_, value);
	}

	{
		unsigned int value = 564658465u;
		CHECK( fixture->setProperty<unsigned int>(
			fixture->uintegerProperty_.get(), provider, value ) );
		CHECK_EQUAL(564658465u, subject_.uinteger_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_float_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.floating_ = 367.345f;

		float value;
		Variant variant = 
			fixture->floatProperty_->get(provider);
		variant.tryCast( value );
		CHECK_EQUAL(367.345f, subject_.floating_);
		CHECK_EQUAL(subject_.floating_, value);
	}

	{
		float value = -321.587f;
		CHECK( fixture->setProperty<float>( 
			fixture->floatProperty_.get(), provider, value ) );
		CHECK_EQUAL(-321.587f, subject_.floating_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_string_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.string_ = BW::string("Hello World!");

		BW::string value;
		Variant variant =
		fixture->stringProperty_->get(provider);
		variant.tryCast( value );

		CHECK_EQUAL(subject_.string_, value);
	}

	{
		BW::string value = "Delicious Cupcakes";
		CHECK( fixture->setProperty<BW::string>( 
			fixture->stringProperty_.get(), provider, value ) );
		CHECK_EQUAL(value, subject_.string_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_wstring_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.wstring_ = BW::wstring(L"Chunky Bacon!");

		BW::wstring value;
		Variant variant = 
			fixture->wstringProperty_->get(provider);
		variant.tryCast( value );

		// cppunitlite wants to serialise the expected and actual values
		// via a std::stringstream, hence wide character strings don't work.

		//CHECK_EQUAL(subject_.wstring_, value);
		CHECK(subject_.wstring_ == value);
	}

	{
		BW::wstring value = L"Foxes driving pickups";
		CHECK( fixture->setProperty<BW::wstring>( 
			fixture->wstringProperty_.get(), provider, value ) );
		//CHECK_EQUAL(value, subject_.wstring_);
		CHECK(value == subject_.wstring_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_vector3_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.vector3_ = BW::Vector3(1.0f, 1.0f, 1.0f);

		BW::Vector3 value;
		Variant variant = 
			fixture->vector3Property_->get(provider);
		variant.tryCast( value );
		CHECK_EQUAL(BW::Vector3(1.0f, 1.0f, 1.0f), subject_.vector3_);
		CHECK_EQUAL(subject_.vector3_, value);
	}

	{
		BW::Vector3 value = BW::Vector3(-1.0f, -1.0f, -1.0f);
		CHECK( fixture->setProperty<BW::Vector3>( 
			fixture->vector3Property_.get(), provider, value ) );
		CHECK_EQUAL(BW::Vector3(-1.0f, -1.0f, -1.0f), subject_.vector3_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_vector4_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.vector4_ = BW::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		BW::Vector4 value;
		Variant variant = 
			fixture->vector4Property_->get(provider);
		variant.tryCast( value );

		CHECK_EQUAL(BW::Vector4(1.0f, 1.0f, 1.0f, 1.0f), subject_.vector4_);
		CHECK_EQUAL(subject_.vector4_, value);
	}

	{
		BW::Vector4 value = BW::Vector4(-1.0f, -1.0f, -1.0f, -1.0f);
		CHECK( fixture->setProperty<BW::Vector4>( 
			fixture->vector4Property_.get(), provider, value ) );
		CHECK_EQUAL(BW::Vector4(-1.0f, -1.0f, -1.0f, -1.0f), subject_.vector4_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_binary_data_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		const char * randomData = "Something evil this way comes.";
		subject_.binary_data_ = std::make_shared< BinaryBlock >(randomData, strlen(randomData) + 1, false );

		std::shared_ptr< BinaryBlock > value;
		Variant variant = 
			fixture->binaryDataProperty_->get(provider);
		variant.tryCast( value );
		CHECK(subject_.binary_data_->compare( *value ) == 0);
	}

	{
		const char * randomData = "Oh no, the boost library is here.";
		auto value = std::make_shared< BinaryBlock >(randomData, strlen(randomData) + 1, false );
		CHECK( fixture->setProperty<decltype( value ) >( 
			fixture->binaryDataProperty_.get(), provider, value ) );
		CHECK(value->compare( *(subject_.binary_data_) ) == 0);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_exposed_struct_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;

	typedef TestExposedStruct TestStruct;

	ObjectHandle baseProvider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );


	Variant vStruct = fixture->exposedStructProperty_->get(
		baseProvider);

	ObjectHandle structProvider;
	vStruct.tryCast( structProvider );

	auto testStruct = structProvider.getBase< TestStruct >();
	CHECK( testStruct != nullptr );
	if (testStruct == nullptr)
	{
		return;
	}
	CHECK_EQUAL( subject_.exposedStruct_.boolean_, testStruct->boolean_ );

	TestStruct value;
	value.boolean_ = !testStruct->boolean_;
	CHECK( fixture->setProperty<TestStruct>( 
		fixture->exposedStructProperty_.get(), baseProvider, value ) );
	CHECK_EQUAL( value.boolean_, subject_.exposedStruct_.boolean_ );
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_exposed_poly_struct_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		subject_.exposedPolyStruct_->string_ = BW::string( "Hello World!" );

		Variant variant =
			fixture->exposedPolyStructProperty_->get( provider);
		ObjectHandle structProvider;
		variant.tryCast( structProvider );
		ReflectedPolyStruct * pStruct = structProvider.getBase<ReflectedPolyStruct >();

		auto value = ReflectionUtilities::dynamicCast< FIXTURE::TestPropertyObject::ExposedPolyStruct >( pStruct );
		CHECK_EQUAL( subject_.exposedPolyStruct_->string_, value->string_ );
	}

	{
		auto & defManager 
			= TestPropertyFixtureBase::getFixture().getDefinitionManager();
		ObjectHandleT< FIXTURE::TestPropertyObject::ExposedPolyStruct > pPs(
			defManager.create< FIXTURE::TestPropertyObject::ExposedPolyStruct >() );
		pPs->string_ = "Delicious Cupcakes";
		CHECK( fixture->setProperty< ObjectHandleT< FIXTURE::TestPropertyObject::ExposedPolyStruct > >( 
			fixture->exposedPolyStructProperty_.get(), provider, pPs ) );
		CHECK_EQUAL( pPs->string_, subject_.exposedPolyStruct_->string_ );
	}
}


// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_link_property( FIXTURE* fixture, const char * m_name, TestResult& result_ )
{
	FIXTURE::TestPropertyObject subject_;
	ObjectHandle provider(
		subject_,
		fixture->getDefinitionManager().getDefinition< FIXTURE::TestPropertyObject >() );

	{
		auto & defManager 
			= TestPropertyFixtureBase::getFixture().getDefinitionManager();
		subject_.link_ = defManager.create< FIXTURE::TestPropertyObject::ExposedPolyStruct >();

		Variant variant =
			fixture->linkProperty_->get( provider );
		ObjectHandle linkProvider;
		variant.tryCast( linkProvider );
		ReflectedPolyStruct * pStruct = linkProvider.getBase< ReflectedPolyStruct >();

		auto value = ReflectionUtilities::dynamicCast< FIXTURE::TestPropertyObject::ExposedPolyStruct >( pStruct );
		CHECK_EQUAL( subject_.link_.get(), value );
	}
}

#define CREATE_PROPERTY( name, getterFunc, setterFunc )	\
	FunctionPropertyHelper<TestPropertyObject>::getBaseProperty( name, getterFunc, setterFunc )

#define CREATE_PROPERTIES( TYPE )												\
	booleanProperty_.reset( CREATE_PROPERTY( "boolean",							\
		&TestPropertyObject::getBoolean##TYPE,									\
		&TestPropertyObject::setBoolean ) );									\
	integerProperty_.reset( CREATE_PROPERTY( "integer",							\
		&TestPropertyObject::getInteger##TYPE,									\
		&TestPropertyObject::setInteger ) );									\
	uintegerProperty_.reset( CREATE_PROPERTY( "unsigned integer",				\
		&TestPropertyObject::getUInteger##TYPE,									\
		&TestPropertyObject::setUInteger ) );									\
	floatProperty_.reset( CREATE_PROPERTY( "float",								\
		&TestPropertyObject::getFloat##TYPE,									\
		&TestPropertyObject::setFloat ) );										\
	stringProperty_.reset( CREATE_PROPERTY( "string",							\
		&TestPropertyObject::getString##TYPE,									\
		&TestPropertyObject::setString ) );										\
	wstringProperty_.reset( CREATE_PROPERTY( "wstring",							\
		&TestPropertyObject::getWString##TYPE,									\
		&TestPropertyObject::setWString ) );									\
	vector3Property_.reset( CREATE_PROPERTY( "vector3",							\
		&TestPropertyObject::getVector3##TYPE,									\
		&TestPropertyObject::setVector3 ) );									\
	vector4Property_.reset( CREATE_PROPERTY( "vector4",							\
		&TestPropertyObject::getVector4##TYPE,									\
		&TestPropertyObject::setVector4 ) );									\
	binaryDataProperty_.reset( CREATE_PROPERTY( "binary data",					\
		&TestPropertyObject::getBinaryData##TYPE,								\
		&TestPropertyObject::setBinaryData ) );									\
	exposedStructProperty_.reset( CREATE_PROPERTY( "exposed struct",			\
		&TestPropertyObject::getExposedStruct##TYPE,							\
		&TestPropertyObject::setExposedStruct ) );								\
	exposedPolyStructProperty_.reset( CREATE_PROPERTY( "exposed poly struct",	\
		&TestPropertyObject::getExposedPolyStruct##TYPE,						\
		&TestPropertyObject::setExposedPolyStruct ) );							\
	linkProperty_.reset( CREATE_PROPERTY( "link property",						\
		&TestPropertyObject::getLink##TYPE,										\
		&TestPropertyObject::setLink ) );

// =============================================================================

class TestPropertyFixtureValue : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureValue()
	{
		CREATE_PROPERTIES( Value )
	}
};

// -----------------------------------------------------------------------------
#define X( TEST ) IMPLEMENT_TEST_F( TestPropertyFixtureValue, TEST )
TESTS
#undef X

// =============================================================================

class TestPropertyFixtureRef : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureRef()
	{
		CREATE_PROPERTIES( Ref )
	}
};

// -----------------------------------------------------------------------------
#define X( TEST ) IMPLEMENT_TEST_F( TestPropertyFixtureRef, TEST )
TESTS
#undef X

// =============================================================================

class TestPropertyFixtureArg : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureArg()
	{
		CREATE_PROPERTIES( Arg )
	}
};

// -----------------------------------------------------------------------------
#define X( TEST ) IMPLEMENT_TEST_F( TestPropertyFixtureArg, TEST )
TESTS
#undef X