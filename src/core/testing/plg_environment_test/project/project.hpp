#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "core_reflection/reflected_object.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <string>



class Project
{
	DECLARE_REFLECTED
public:
	Project();
	~Project();

private:
	void setCheckBoxState( const bool & bChecked ) ;
	void getCheckBoxState( bool * bChecked ) const;

	void setTextField( const std::wstring & text );
	void getTextField( std::wstring * text ) const;

	void setSlideData(const double & length);
	void getSlideData(double * length) const;

	static int getSlideMaxData();
	static int getSlideMinData();

	void setNumber( const int & num );
	void getNumber( int * num ) const;

	void setSelected( const int & select );
	void getSelected( int * select ) const;

	void setVector3(const Vector3 & vec3);
	void getVector3(Vector3 * vec3) const;

	void setVector4(const Vector4 & vec4);
	void getVector4(Vector4 * vec4) const;

	void setColor3(const Vector3 & color);
	void getColor3(Vector3 * color) const;

	void setColor4(const Vector4 & color);
	void getColor4(Vector4 * color) const;

	
	bool bChecked_;
	std::wstring text_;
	double curSlideData_;
	int curNum_;
	int curSelected_;
	int enumValue_;
	Vector3 vec3_;
	Vector4 vec4_;
	Vector3 color3_;
	Vector4 color4_;
};



#endif // PROJECT_HPP
