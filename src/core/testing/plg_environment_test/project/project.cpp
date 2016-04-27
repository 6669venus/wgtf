#include "project.hpp"


Project::Project()
	: bChecked_( false)
	, text_( L"Hello Test" )
	, curSlideData_( 0 )
	, curNum_( 100 )
	, curSelected_( 0 )
	, enumValue_( 0 )
	, vec3_(0.0f, 0.0f, 0.0f)
	, vec4_(0.0f, 0.0f, 0.0f, 0.0f)
	, color3_(192.0f, 192.0f, 192.0f)
	, color4_(127.0f, 127.0f, 127.0f, 127.0f)
{
}

Project::~Project()
{
}

void Project::setCheckBoxState( const bool & bChecked )
{
	bChecked_ = bChecked;
}
void Project::getCheckBoxState( bool * bChecked ) const
{
	*bChecked = bChecked_;
}

void Project::setTextField( const std::wstring & text )
{
	text_ = text;
}
void Project::getTextField( std::wstring * text ) const
{
	*text = text_;
}

void Project::setSlideData( const double & length )
{
	if ((length < this->getSlideMinData()) || (length > this->getSlideMaxData()))
	{
		return;
	}
	curSlideData_ = length;
}
void Project::getSlideData(double * length) const
{
	*length = curSlideData_;
}

int Project::getSlideMaxData()
{
	return 100;
}
int Project::getSlideMinData()
{
	return -100;
}

void Project::setNumber( const int & num )
{
	curNum_ = num;
}
void Project::getNumber( int * num ) const
{
	*num = curNum_;
}

void Project::setSelected( const int & select )
{
	curSelected_ = select;
}
void Project::getSelected( int * select ) const
{
	*select = curSelected_;
}

void Project::setVector3(const Vector3 & vec3)
{
	vec3_.x = vec3.x;
	vec3_.y = vec3.y;
	vec3_.z = vec3.z;
}
void Project::getVector3(Vector3 * vec3) const
{
	vec3->x = vec3_.x;
	vec3->y = vec3_.y;
	vec3->z = vec3_.z;
}

void Project::setVector4(const Vector4 & vec4)
{
	vec4_.x = vec4.x;
	vec4_.y = vec4.y;
	vec4_.z = vec4.z;
	vec4_.w = vec4.w;
}
void Project::getVector4(Vector4 * vec4) const
{
	vec4->x = vec4_.x;
	vec4->y = vec4_.y;
	vec4->z = vec4_.z;
	vec4->w = vec4_.w;
}

void Project::setColor3(const Vector3 & color)
{
	color3_.x = color.x;
	color3_.y = color.y;
	color3_.z = color.z;
}
void Project::getColor3(Vector3 * color) const
{
	color->x = color3_.x;
	color->y = color3_.y;
	color->z = color3_.z;
}

void Project::setColor4(const Vector4 & color)
{
	color4_.x = color.x;
	color4_.y = color.y;
	color4_.z = color.z;
	color4_.w = color.w;
}
void Project::getColor4(Vector4 * color) const
{
	color->x = color4_.x;
	color->y = color4_.y;
	color->z = color4_.z;
	color->w = color4_.w;
}

