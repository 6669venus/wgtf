#include "pch.hpp"

#include "reflection/reflected_object.hpp"
#include "reflection/serializer/base_serializer.hpp"

namespace wgt
{
BW_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
class TestDefinitionObject : public ReflectedObject
{
	DECLARE_REFLECTED( TestDefinitionObject )

public:
	int counter_;

	BW::string text_;

	Vector3 position_;

	BW::vector<float> values_;

	void setCounter( const int & value ) { counter_ = value; }
	void getCounter( int * value ) const { *value = counter_; }

	void setText( const BW::string & value ) { text_ = value; }
	const BW::string & getText() const { return text_; }

	bool operator==( const TestDefinitionObject& tdo ) const
	{
		if ( counter_ != tdo.counter_ || text_ != tdo.text_ || 
			position_ != tdo.position_ || values_.size() != tdo.values_.size() )
		{
			return false;
		}

		int i = 0;
		for (; i < values_.size() && values_[i] == tdo.values_[i]; ++i);
		return i == values_.size();
	}
};


//------------------------------------------------------------------------------
class TestDefinitionFixture : public IReflectedObjectFactory
{
public:
	// IReflectedObjectFactory
	virtual ReflectedObjectPtr createObject( 
		const RefObjectId & id,
		const BW::string & classDef ) const
	{
		// ignores id for tests only!
		if (classDef == klass_.getName())
		{
			return klass_.createObject();
		}
		return NULL;
	}

	TestDefinitionFixture() :
		klass_(TestDefinitionObject::getStaticDefinition())
	{
	}

	void fillValuesWithNumbers(BW::vector<float>& values)
	{
		float increment = 3.25f;
		float value = 1.0f;
		for (int i = 0; i < 5; ++i)
		{
			values.push_back(value);
			value += increment;
			increment += 3.25f;
		}
	}

public:
	ClassDefinition& klass_;
};

BW_END_NAMESPACE

}
