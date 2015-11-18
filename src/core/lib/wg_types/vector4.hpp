#ifndef VECTOR4_HPP
#define VECTOR4_HPP

class Vector4
{
public:
	Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	Vector4(const Vector3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
	Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
	float x;
	float y;
	float z;
	float w;

	bool operator == (const Vector4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator != (const Vector4& v) const
	{
		return !(*this == v);
	}

	bool operator < (const Vector4& v) const
	{
		if (x < v.x) return true;
		if (x > v.x) return false;
		if (y < v.y) return true;
		if (y > v.y) return false;
		if (z < v.z) return true;
		if (z > v.z) return false;
		return (w < v.w);
	}
};

#endif // VECTOR3_HPP

