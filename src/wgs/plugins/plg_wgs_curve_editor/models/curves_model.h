//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curves_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef CURVES_MODEL_H_
#define CURVES_MODEL_H_

#pragma once

#include "reflection/reflected_object.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include <QtCore/QPoint>

class Point
{
	DECLARE_REFLECTED
public:
	Point() : x(0.f), y(0.f){}
	Point(float x, float y) : x(x), y(y){}

	float getX() const { return x; }
	void setX(const float& newX) { x = newX; }
	float getY() const { return y; }
	void setY(const float& newY) { y = newY; }

	Point& operator*=(float val)
	{
		x *= val;
		y *= val;
		return *this;
	}

	Point& operator/=(float val)
	{
		x /= val;
		y /= val;
		return *this;
	}

	Point& operator+=(const Point& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Point& operator-=(const Point& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Point operator*(float val) const
	{
		return Point(x * val, y * val);
	}

	Point operator/(float val) const
	{
		return Point(x / val, y / val);
	}

	Point operator-(const Point& other) const
	{
		return Point(x - other.x, y - other.y);
	}

	Point operator+(const Point& other) const
	{
		return Point(x + other.x, y + other.y);
	}

	float x;
	float y;
};
bool operator==(const Point& p1, const Point& p2);
Point operator*(float val, const Point& p1);
Point operator/(float val, const Point& p1);

class BezierPoint
{
	DECLARE_REFLECTED
public:
	BezierPoint() {}
	BezierPoint(float x, float y, float cp1x = 0.0f, float cp1y = 0.0f, float cp2x = 0.0f, float cp2y = 0.0f)
		: pos(x, y), cp1(cp1x, cp1y), cp2(cp2x, cp2y)
	{}

	bool operator==(const BezierPoint& other) const
	{
		return pos == other.pos && cp1 == other.cp1 && cp2 == other.cp2;
	}
	
	Point pos;
	Point cp1;
	Point cp2;
};

class Curve
{
	DECLARE_REFLECTED
public:
	Curve(){}
	Curve(const Curve& other)
	{
		for (auto& p : other.points_)
		{
			points_.push_back(p.getData(0, ValueRole::roleId_));
		}
	}
	template<class TCollection>
	Curve(const TCollection& pts)
	{
		for (auto& p : pts)
		{
			points_.push_back(p);
		}
	}

	Curve& operator=(const Curve& other)
	{
		points_.clear();
		for (auto& p : other.points_)
		{
			points_.push_back(p.getData(0, ValueRole::roleId_));
		}
		return *this;
	}

	float dummy() const { return 0; }

	template<class TVal>
	TVal computeValueAtT(float t, TVal p1, TVal c1, TVal c2, TVal p2)
	{
		auto t2 = t*t;
		auto t3 = t2*t;
		// Cubic Bezier: (1-t)^3 * P1 + 3(1-t)^2 * t * C1 + 3*(1-t)*t^2*C2 + t^3*P2
		auto invT = (1 - t);
		auto invT2 = invT*invT;
		auto invT3 = invT2*invT;
		return invT3*p1 + 3.f * invT2*t*c1 + 3.f * invT*t2*c2 + t3*p2;
	}

	float tAtX(float x, const BezierPoint& prevPoint, const BezierPoint& nextPoint)
	{
		auto t = (x - prevPoint.pos.x) / (nextPoint.pos.x - prevPoint.pos.x);
		const float desiredTime = x;
		x = computeValueAtT(t, prevPoint.pos.x, prevPoint.pos.x + prevPoint.cp2.x, nextPoint.pos.x + nextPoint.cp1.x, nextPoint.pos.x);
		float dist, last;

		float curError = std::numeric_limits<float>::max(), lastError = fabs((x / desiredTime) - 1);
		static const float kWalker = 0.5f;
		static const float kMaxError = 0.00025f;

		do {
			last = x;

			dist = (desiredTime - x) * kWalker;
			t += dist;

			x = computeValueAtT(t, prevPoint.pos.x, prevPoint.pos.x + prevPoint.cp2.x, nextPoint.pos.x + nextPoint.cp1.x, nextPoint.pos.x);

			lastError = curError;
			if (desiredTime == 0.f)
			{
				curError = 0.f;
			}
			else
			{
				curError = fabs((x / desiredTime) - 1);
			}
		} while (curError < lastError && curError > kMaxError);

		return t;
	}

	void add(const float& x)
	{
		auto iter = std::begin(points_);
		BezierPoint newPoint(x, 0);
		BezierPoint* prevPoint = nullptr;
		for (; iter != std::end(points_); ++iter)
		{
			auto& point = *(*iter).getData(0, ValueRole::roleId_).castRef<ObjectHandle>().getBase<BezierPoint>();
			if (point.pos.x > x)
			{
				newPoint.pos.y = point.pos.y;
				if (prevPoint)
				{
					auto t = tAtX(x, *prevPoint, point);
					auto p1 = prevPoint->pos, c1 = prevPoint->pos + prevPoint->cp2;
					auto p2 = point.pos, c2 = point.pos + point.cp1;
					newPoint.pos = computeValueAtT(t, p1, c1, c2, p2);
					// Using DeCastlejau's to compute new control points
					auto cp1 = (c1 - p1)*t + p1;
					auto cp2 = (c2 - c1)*t + c1;
					auto cp3 = (p2 - c2)*t + c2;
					newPoint.cp1 = (cp2 - cp1)*t + cp1 - newPoint.pos;
					newPoint.cp2 = (cp3 - cp2)*t + cp2 - newPoint.pos;
					prevPoint->cp2 *= t;
					point.cp1 *= 1.f - t;
				}
				else
				{
					point.cp1.x = (x - point.pos.x) / 2.f;
					point.cp1.y = 0;
				}
				break;
			}
			prevPoint = &point;
		}
		if (iter == std::end(points_))
		{
			newPoint.pos.y = prevPoint ? prevPoint->pos.y : 0;
			if (prevPoint)
			{
				prevPoint->cp2.x = (newPoint.pos.x - prevPoint->pos.x) / 2.f;
				prevPoint->cp2.y = 0.f;
			}
		}
		(*points_.insert(iter, newPoint)).getData(0, ValueRole::roleId_);
	}

	void remove(const float& x)
	{
		for (auto iter = std::begin(points_); iter != std::end(points_); ++iter)
		{
			auto& point = *(*iter).getData(0, ValueRole::roleId_).castRef<ObjectHandle>().getBase<BezierPoint>();
			if (point.pos.x == x)
			{
				points_.erase(iter);
				break;
			}
		}
	}

	bool operator==(const Curve& other) const
	{
		for (auto& p : points_)
		{
			for (auto& op : other.points_)
			{
				if (!(p == op))
					return false;
			}
		}
		return true;
	}

	ObjectHandle points() const { return points_; }
private:
	GenericList points_;
};

class CurvesModel
{
	DECLARE_REFLECTED
public:
	CurvesModel(){}
	CurvesModel(const CurvesModel& other)
	{
		for (auto& c : other.curves_)
		{
			curves_.push_back(c.getData(0, ValueRole::roleId_));
		}
	}
	void append(Curve& curve)
	{
		curves_.push_back(curve);
	}
	ObjectHandle getCurves() const { return curves_; }

private:
	GenericList curves_;
};


#endif // CURVES_MODEL_H_