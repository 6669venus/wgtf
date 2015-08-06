#ifndef TEST_PAGE_HPP
#define TEST_PAGE_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "wg_types/binary_block.hpp"
#include <map>
#include <vector>
#include <memory>

class TestPolyStruct;

typedef ObjectHandleT< TestPolyStruct >  ReflectedPolyStructPtr;

class TestPage
	: public ReflectedPolyStruct
{
	DECLARE_REFLECTED
public:
	TestPage();
	~TestPage();
	void init();

private:
	void setCheckBoxState( const bool & bChecked ) ;
	void getCheckBoxState( bool * bChecked ) const;

	void setTextField( const std::wstring & text );
	void getTextField( std::wstring * text ) const;

	void setSlideData( const int & length );
	void getSlideData( int * length ) const;

	static int getSlideMaxData();
	static int getSlideMinData();

	void setNumber( const int & num );
	void getNumber( int * num ) const;

	void setSelected( const int & select );
	void getSelected( int * select ) const;

	void getThumbnail( std::shared_ptr< BinaryBlock > * path ) const;


	const GenericObjectPtr & getGenericObject() const;
	void setGenericObject( const GenericObjectPtr & genericObj );

	void setTestPolyStruct( const ReflectedPolyStructPtr & testPolyStruct );
	const ReflectedPolyStructPtr & getTestPolyStruct() const;

	const std::vector< std::vector< float > > & getTestVector() const
	{
		return testVector_;
	}

	void getEnumFunc( int * o_EnumValue ) const
	{
		*o_EnumValue = enumValue_;
	}

	void setEnumFunc( const int & o_EnumValue )
	{
		enumValue_ = o_EnumValue;
	}

	void generateEnumFunc(
		std::map< int, std::wstring > * o_enumMap ) const;

	
	bool bChecked_;
	bool boolTest_;
	std::wstring text_;
	int curSlideData_;
	int curNum_;
	int curSelected_;
	int enumValue_;
	std::vector< int > intVector_;
	std::vector< float > floatVector_;
	std::vector< std::vector< int > > vectorVector_;
	std::vector< std::vector< float > > testVector_;
	std::map< int, std::string > testMap_;
	std::map< int, std::vector< std::string > > testVectorMap_;
	ReflectedPolyStructPtr polyStruct_;
	GenericObjectPtr genericObj_;
	//TODO: More tests to work out collections of
	//ReflectedStructs, ReflectedPolyStructs & ReflectedObjects
};

class TestPage2
{
public:
	TestPage2();
	~TestPage2();
	void init( IDefinitionManager & defManager );
	const ObjectHandleT<TestPage> & getTestPage() const;
	void setTestPage( const ObjectHandleT<TestPage> & objHandle );
private:
	ObjectHandleT<TestPage> testPage_;
};


#endif // TEST_PAGE_IMPL_HPP
