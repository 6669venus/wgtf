#include "test_polymorphism.hpp"
#include "reflection/utilities/reflection_utilities.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/base_property.hpp"

bool isColorEqual( const BW::Vector3 & lfh, const BW::Vector3 & rfh )
{
	if( ((int)(lfh.x) == (int)(rfh.x)) 
		&& ((int)(lfh.y) == (int)(rfh.y)) 
		&& ((int)(lfh.z) == (int)(rfh.z)) )
	{
		return true;
	}
	return false;
}

TestPolyCheckBox::TestPolyCheckBox()
	: checked_( false )
{

}


TestPolyTextField::TestPolyTextField()
	: text_( "BigWorld" )
	, num_( 55 )
{

}

void TestPolyTextField::getText( std::string * text ) const
{
	*text = text_;
}

void TestPolyTextField::setText( const std::string & text )
{
	text_ = text;
}

void TestPolyTextField::getNumber( int * num ) const
{
	*num = num_;
}

void TestPolyTextField::setNumber( const int & num )
{
	num_ = num;
}

TestPolyComboBox::TestPolyComboBox()
	: curSelected_( 0 )
{

}

void TestPolyComboBox::getSelected( int * select ) const
{
	*select = curSelected_;
}

void TestPolyComboBox::setSelected( const int & select )
{
	curSelected_ = select;
}

TestPolyColor3::TestPolyColor3()
	: color3_( 192.0f, 192.0f, 192.0f )
	, red_( 255.0f, 0.0f, 0.0f )
	, green_( 0.0f, 255.0f, 0.0f )
	, blue_( 0.0f, 0.0f, 255.0f )
	, seletColor_( 3 )
{

}

void TestPolyColor3::getColor3( BW::Vector3 * color ) const
{
	color->x = color3_.x;
	color->y = color3_.y;
	color->z = color3_.z;
}

void TestPolyColor3::setColor3( const BW::Vector3 & color )
{
	color3_.x = color.x;
	color3_.y = color.y;
	color3_.z = color.z;

	if (isColorEqual( color, red_))
	{
		if(seletColor_ != 0)
		{
			setSelected( 0 );
		}
		return;
	}
	if (isColorEqual( color, green_))
	{
		if(seletColor_ != 1)
		{
			setSelected( 1 );
		}
		return;
	}
	if (isColorEqual( color, blue_))
	{
		if(seletColor_ != 2)
		{
			setSelected( 2 );
		}
		return;
	}
	if(seletColor_ != 3)
	{
		setSelected( 3 );
	}
}

void TestPolyColor3::getSelected( int * select ) const
{
	*select = seletColor_;
}

void TestPolyColor3::setSelected( const int & select )
{
	seletColor_ = select;

	switch( select )
	{
	case 0:
		setColor3( red_ );
		break;
	case 1:
		setColor3( green_ );
		break;
	case 2:
		setColor3( blue_ );
		break;
	default:
		BW::Vector3 custom( 192.0f, 192.0f, 192.0f );
		setColor3( custom);
		break;
	}
}
