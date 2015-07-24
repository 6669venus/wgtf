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
#include "reflection/type_class_definition.hpp"
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
	BezierPoint(ObjectHandle pos, ObjectHandle cp1, ObjectHandle cp2)
		: pos(pos), cp1(cp1), cp2(cp2)
	{}

	bool operator==(const BezierPoint& other) const
	{
		return pos == other.pos && cp1 == other.cp1 && cp2 == other.cp2;
	}
	
	ObjectHandle pos;
	ObjectHandle cp1;
	ObjectHandle cp2;
};

class Curve
{
	DECLARE_REFLECTED
public:
	Curve(){}
	Curve(const Curve& other)
		: definitionManager(other.definitionManager)
	{
		for (auto& p : other.points_)
		{
			points_.push_back(p.getData(0, ValueRole::roleId_));
		}
	}
	template<class TCollection>
	Curve(IDefinitionManager* definitionManager, const TCollection& pts)
		: definitionManager(definitionManager)
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
		const auto& prevPos = *prevPoint.pos.getBase<Point>();
		const auto& prevCp2 = *prevPoint.cp2.getBase<Point>();
		const auto& nextPos = *nextPoint.pos.getBase<Point>();
		const auto& nextCp1 = *nextPoint.cp1.getBase<Point>();
		auto t = (x - prevPos.x) / (nextPos.x - prevPos.x);
		const float desiredTime = x;
		x = computeValueAtT(t, prevPos.x, prevPos.x + prevCp2.x, nextPos.x + nextCp1.x, nextPos.x);
		float dist, last;

		float curError = std::numeric_limits<float>::max(), lastError = fabs((x / desiredTime) - 1);
		static const float kWalker = 0.5f;
		static const float kMaxError = 0.00025f;

		do {
			last = x;

			dist = (desiredTime - x) * kWalker;
			t += dist;

			x = computeValueAtT(t, prevPos.x, prevPos.x + prevCp2.x, nextPos.x + nextCp1.x, nextPos.x);

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
		if(definitionManager == nullptr)
			return;
		auto iter = std::begin(points_);
		auto ptDefinition = definitionManager->getDefinition<Point>();
		auto posHandle = ptDefinition->create();
		auto cp1Handle = ptDefinition->create();
		auto cp2Handle = ptDefinition->create();
		auto bezPtDefinition = definitionManager->getDefinition<BezierPoint>();
		auto newBezPointHandle = TypeClassDefinition<BezierPoint>::create(*bezPtDefinition, posHandle, cp1Handle, cp2Handle);
		auto& newPos = *posHandle.getBase<Point>();
		auto& newCp1 = *cp1Handle.getBase<Point>();
		auto& newCp2 = *cp2Handle.getBase<Point>();
		BezierPoint* prevPoint = nullptr;
		for (; iter != std::end(points_); ++iter)
		{
			auto& point = *(*iter).getData(0, ValueRole::roleId_).castRef<ObjectHandle>().getBase<BezierPoint>();
			if (point.pos.getBase<Point>()->x > x)
			{
				newPos.y = point.pos.getBase<Point>()->y;
				if (prevPoint)
				{
					auto t = tAtX(x, *prevPoint, point);
					auto p1 = *prevPoint->pos.getBase<Point>(), c1 = *prevPoint->pos.getBase<Point>() + *prevPoint->cp2.getBase<Point>();
					auto p2 = *point.pos.getBase<Point>(), c2 = *point.pos.getBase<Point>() + *point.cp1.getBase<Point>();
					newPos = computeValueAtT(t, p1, c1, c2, p2);
					// Using DeCastlejau's to compute new control points
					auto cp1 = (c1 - p1)*t + p1;
					auto cp2 = (c2 - c1)*t + c1;
					auto cp3 = (p2 - c2)*t + c2;
					cp1 = (cp2 - cp1)*t + cp1 - newPos;
					cp2 = (cp3 - cp2)*t + cp2 - newPos;
					*prevPoint->cp2.getBase<Point>() *= t;
					*point.cp1.getBase<Point>() *= 1.f - t;
				}
				else
				{
					point.cp1.getBase<Point>()->x = (x - point.pos.getBase<Point>()->x) / 2.f;
					point.cp1.getBase<Point>()->y = 0;
				}
				break;
			}
			prevPoint = &point;
		}
		if (iter == std::end(points_))
		{
			newPos.y = prevPoint ? prevPoint->pos.getBase<Point>()->y : 0;
			if (prevPoint)
			{
				prevPoint->cp2.getBase<Point>()->x = (newPos.x - prevPoint->pos.getBase<Point>()->x) / 2.f;
				prevPoint->cp2.getBase<Point>()->y = 0.f;
			}
		}
		(*points_.insert(iter, newBezPointHandle)).getData(0, ValueRole::roleId_);
	}

	void remove(const float& x)
	{
		for (auto iter = std::begin(points_); iter != std::end(points_); ++iter)
		{
			auto& point = *(*iter).getData(0, ValueRole::roleId_).castRef<ObjectHandle>().getBase<BezierPoint>();
			if (point.pos.getBase<Point>()->x == x)
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
	IDefinitionManager* definitionManager;
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
	void append(ObjectHandle curve)
	{
		curves_.push_back(curve);
	}
	ObjectHandle getCurves() const { return curves_; }

private:
	GenericList curves_;
};


#endif // CURVES_MODEL_H_