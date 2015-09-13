#ifndef XBOUND_H
#define XBOUND_H
#include <vector>
#include "Box2D.h"
/*
enum BOUND_TYPE
{
	BOUND_UNKNOWN,
	BOUND_RECT,
	BOUND_CIRCLE,
	BOUND_CONVEX_POLYGON,
};
class Bound
{
	b2Shape* shape;
public:
	virtual BOUND_TYPE GetType()
	{
		return BOUND_UNKNOWN;
	}
	virtual int GetLeft() = 0;
	virtual int GetRight() = 0;
	virtual int GetTop() = 0;
	virtual int GetBottom() = 0;
};
class BoundRectangle: public Bound
{
public:
	int x, y, w, h;
	virtual BOUND_TYPE GetType()
	{
		return BOUND_RECT;
	}
	virtual int GetLeft()
	{
		return x;
	}
	virtual int GetRight()
	{
		return x+w;
	}
	virtual int GetTop()
	{
		return y;
	}
	virtual int GetBottom()
	{
		return y+h;
	}
};
class BoundCircle: public Bound
{
public:
	int x, y, r;
	virtual BOUND_TYPE GetType()
	{
		return BOUND_CIRCLE;
	}
	virtual int GetLeft()
	{
		return x-r;
	}
	virtual int GetRight()
	{
		return x+r;
	}
	virtual int GetTop()
	{
		return y-r;
	}
	virtual int GetBottom()
	{
		return y+r;
	}
};
*/
class BoundGroup
{
private:
	int width, height, left, right, top, bottom;
	b2Body* body;
	b2BodyDef bodyDef;
public:
	enum BoundGroupType
	{
		BG_TYPE_UNKNOWN = -1,
		BG_TYPE_BODY,
		BG_TYPE_WEAPON,
		BG_TYPE_COUNT,
	};
	//int id;
	std::vector<b2Shape*> shapeList;
	/*void Init();

	int GetWidth()
	{
		return width;
	}
	int GetHeight()
	{
		return height;
	}*/
	bool IsIntersectedWith(BoundGroup* g, b2Transform* t1, b2Transform* t2);
};
#endif