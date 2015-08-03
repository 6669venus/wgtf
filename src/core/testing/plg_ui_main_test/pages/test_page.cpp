#include "test_page.hpp"
#include "interfaces/i_datasource.hpp"
#include "pages/test_polymorphism.hpp"
#include "reflection/i_object_manager.hpp"
TestPage::TestPage()
	: bChecked_( true )
	, boolTest_( false)
	, text_( L"Hello Test" )
	, curSlideData_( 0 )
	, curNum_( 100 )
	, curSelected_( 0 )
	, enumValue_( 0 )
	, polyStruct_( nullptr )
	, genericObj_( nullptr )
{
	for( auto i = 0; i < 10; ++i)
	{
		intVector_.push_back( rand() );
	}

	for( auto i = 0; i < 10; ++i)
	{
		std::vector< float > bla;
		for( auto i = 0; i < 10; ++i)
		{
			bla.push_back( (float ) rand()  / RAND_MAX );
		}
		testVector_.push_back( bla );
	}

	for( auto i = 0; i < 10; ++i)
	{
		floatVector_.push_back( ( float ) rand() / RAND_MAX );
	}
	for( auto i = 0; i < 10; ++i)
	{
		std::vector< int > bla;
		for( auto j = 0; j < 10; ++j)
		{
			bla.push_back( rand() );
		}
		vectorVector_.push_back( bla );
	}

	std::string testString;
	for( auto i = 0; i < 10; ++i)
	{
		char testBuffer[ 1024 ];
		char * pos = &testBuffer[ 0 ];
		for( auto j = 0; j < 10; ++j)
		{
			pos += sprintf( pos, "%d%c", rand(), ( char ) ( ( float ) rand() / RAND_MAX * 26 + 'a' ) );
		}
		testString = testBuffer;
		testMap_.insert ( std::make_pair( rand(), testString ) );
	}

	for( auto i = 0; i < 10; ++i)
	{
		std::vector< std::string > stringVector;
		for( auto j = 0; j < 10; ++j)
		{
			char testBuffer[ 1024 ];
			char * pos = &testBuffer[ 0 ];
			for( auto k = 0; k < 10; ++k)
			{
				pos += sprintf( pos, "%d%c", rand(), ( char ) ( ( float ) rand() / RAND_MAX * 26 + 'a' ) );
			}
			stringVector.push_back( testBuffer );
		}
		testVectorMap_.insert ( std::make_pair( rand(), stringVector ) );
	}
}

TestPage::~TestPage()
{
	polyStruct_ = nullptr;
}

void TestPage::init()
{
	if (polyStruct_ == nullptr)
	{
		auto defManager = getDefinition().getDefinitionManager();
		polyStruct_ = 
			defManager->create<TestPolyStruct>( false ); 
		polyStruct_->init( *defManager );
	}
	if (genericObj_ == nullptr)
	{
		auto defManager = getDefinition().getDefinitionManager();
		genericObj_ = GenericObject::create( *defManager );
		genericObj_->set( "String", std::string( "Wargaming" ) );
		genericObj_->set( "Integer", 100 );
	}
}

void TestPage::setCheckBoxState( const bool & bChecked )
{
	bChecked_ = bChecked;
}
void TestPage::getCheckBoxState( bool * bChecked ) const
{
	*bChecked = bChecked_;
}

void TestPage::setTextField( const std::wstring & text )
{
	text_ = text;
}
void TestPage::getTextField( std::wstring * text ) const
{
	*text = text_;
}

void TestPage::setSlideData( const int & length )
{
	if (( length < -100 ) || ( length > 100 ))
	{
		return;
	}
	curSlideData_ = length;
}
void TestPage::getSlideData( int * length ) const
{
	*length = curSlideData_;
}

int TestPage::getSlideMaxData()
{
	return 100;
}
int TestPage::getSlideMinData()
{
	return -100;
}

void TestPage::setNumber( const int & num )
{
	curNum_ = num;
}
void TestPage::getNumber( int * num ) const
{
	*num = curNum_;
}

void TestPage::setSelected( const int & select )
{
	curSelected_ = select;
}
void TestPage::getSelected( int * select ) const
{
	*select = curSelected_;
}

void TestPage::getThumbnail( std::shared_ptr< BinaryBlock > * thumbnail ) const
{
	auto dataSrc = Context::queryInterface< IDataSource >();
	assert( dataSrc );
	*thumbnail = dataSrc->getThumbnailImage();
}

const GenericObjectPtr & TestPage::getGenericObject() const
{
	return genericObj_;
}

void TestPage::setGenericObject( const GenericObjectPtr & genericObj )
{
	genericObj_ = genericObj;
}

void TestPage::setTestPolyStruct( const ReflectedPolyStructPtr & testPolyStruct )
{
	polyStruct_ = testPolyStruct;
}
const ReflectedPolyStructPtr & TestPage::getTestPolyStruct() const
{
	return polyStruct_;
}

void TestPage::generateEnumFunc(
	std::map< int, std::wstring > * o_enumMap ) const
{
	o_enumMap->insert( std::make_pair( 0, L"First Value" ) );
	o_enumMap->insert( std::make_pair( 1, L"Second Value" ) );
	o_enumMap->insert( std::make_pair( 2, L"third Value" ) );
	o_enumMap->insert( std::make_pair( 3, L"Forth Value" ) );
}

TestPage2::TestPage2()
	: testPage_( nullptr )
{

}

TestPage2::~TestPage2()
{
	testPage_ = nullptr;
}

void TestPage2::init( IDefinitionManager & defManager )
{
	assert( testPage_ == nullptr );
	testPage_ = defManager.create<TestPage>( false );
	testPage_->init();
}

const ObjectHandleT<TestPage> & TestPage2::getTestPage() const
{
	return testPage_;
}

void TestPage2::setTestPage( const ObjectHandleT<TestPage> & objHandle )
{
	testPage_ = objHandle;
}
