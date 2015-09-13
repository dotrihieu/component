#include "Bound.h"
#include <math.h>

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);
/*
void BoundGroup::Init()
{
	width = 0;
	height = 0;
	left = 0;
	right = 0;
	top = 0;
	bottom = 0;
	std::vector<Bound*>::iterator it = boundList.begin();
	if(it != boundList.end())
	{
		left = (*it)->GetLeft();
		right = (*it)->GetRight();
		top = (*it)->GetTop();
		bottom = (*it)->GetBottom();
	}
	for(++it; it != boundList.end(); ++it)
	{
		left = std::min(left, (*it)->GetLeft());
		right = std::max(right, (*it)->GetRight());
		top = std::min(top, (*it)->GetTop());
		bottom = std::max(bottom, (*it)->GetBottom());
	}
	width = right - left;
	height = bottom - top;
	bodyDef.position.Set(0.0f,0.0f);
	body = world.CreateBody(&bodyDef);
	for(std::vector<Bound*>::iterator it = boundList.begin(); it != boundList.end(); ++it)
	{
		switch ((*it)->GetType())
		{
		case BOUND_TYPE::BOUND_CIRCLE:
			{
				b2CircleShape shape;
				BoundCircle* circle = (BoundCircle*)(*it);
				shape.m_p.Set(circle->x, circle->y);
				shape.m_radius = circle->r;
				body->CreateFixture(&shape, 0.0f);
			}
			break;
		case BOUND_TYPE::BOUND_RECT:
			{
				b2PolygonShape shape;
				BoundRectangle* rect = (BoundRectangle*)(*it);
				shape.SetAsBox(rect->w/2,rect->h/2, b2Vec2(rect->x, rect->y), 0.0f);	
				body->CreateFixture(&shape, 0.0f);
			}
			break;
		case BOUND_TYPE::BOUND_CONVEX_POLYGON:
			break;
		default:
			break;
		}
	}
}
*/
bool BoundGroup::IsIntersectedWith(BoundGroup* g, b2Transform* t1, b2Transform* t2)
{
	for(std::vector<b2Shape*>::iterator it = shapeList.begin(); it != shapeList.end(); ++it)
	{
		for(std::vector<b2Shape*>::iterator it2 = g->shapeList.begin(); it2 != g->shapeList.end(); ++it2)
		{
			if(b2TestOverlap(*it, 0, *it2, 0, *t1, *t2))
			{
				return true;
			}
		}
	}
	return false;
}