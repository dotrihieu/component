#ifndef XSCREEN_H
#define XSCREEN_H
#include "Common.h"
#include "XMath.h"
class Screen : public Singleton<Screen>
{
	int width;
	int height;
	float ratio; //width/height
	float ratioScreen_Logical;
	RectScreenF screenRect;
	Rect3DF screenRectLogical;
public:
	Screen(int w, int h)
	{
		width = w;
		height = h;
		ratio = float(w)/float(h);
		screenRect.right = float(w)/2.0f;
		screenRect.left = - screenRect.right;
		screenRect.bottom = float(height)/2.0f;
		screenRect.top = -screenRect.bottom;
		screenRectLogical.top = 1.0f;
		screenRectLogical.bottom = -1.0f;
		screenRectLogical.left = -ratio;
		screenRectLogical.right = ratio;
		ratioScreen_Logical = float(h)/2.0f;
	}
	static void Init(int w, int h)
	{
		SafeDelete(instance);
		instance = new Screen(w, h);
	}
	static void Release()
	{
		SafeDelete(instance);
	}
	int GetWidth()
	{
		return width;
	}
	int GetHeight()
	{
		return height;
	}
	float GetRatio()
	{
		return ratio;
	}
	float GetLeftLogical()
	{
		return screenRectLogical.left;
	}
	float GetRightLogical()
	{
		return screenRectLogical.right;
	}
	float GetTopLogical()
	{
		return screenRectLogical.top;
	}
	float GetBottomLogical()
	{
		return screenRectLogical.bottom;
	}
	Vector2f ScreenToLogical(const float& x, const float &y)
	{
		Vector2f result;
		result.x = x / ratioScreen_Logical - ratio;
		result.y = -(y / ratioScreen_Logical - 1.0f);
		return result;
	}
	float LogicalToScreen(float value)
	{
		return value*ratioScreen_Logical;
	}
};
#endif