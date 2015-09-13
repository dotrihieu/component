#ifndef TOUCHMANAGER_H
#define TOUCHMANAGER_H
#include "Common.h"
#include <vector>
#define MAX_TOUCH 2
struct TouchEvent
{
	enum TouchEventState
	{
		None,
		Pressed,
		Move,
		Released
	};
	unsigned int id;
	float x, y;
	TouchEventState state;
	TouchEvent()
	{
		id = 0;
		x = 0.0f;
		y = 0.0f;
		state = TouchEventState::None;
	}
};
struct Touch
{
	enum TouchState
	{
		None,
		Pressed,
		Down,
		Move,
		Released
	};
	unsigned int id;
	float x, y, xPrev, yPrev;
	TouchState state;
	Touch()
	{
		id = 0;
		x = 0.0f;
		y = 0.0f;
		xPrev = 0.0f;
		yPrev = 0.0f;
		state = TouchState::None;
	}
};
class TouchManager: public Singleton<TouchManager>
{
private:
	Touch touch[MAX_TOUCH];
	std::vector<TouchEvent> touchEventQueue;
public:
	static void Init()
	{
		SafeDelete(instance);
		instance = new TouchManager();
	}
	void OnTouchDown(unsigned int id, float x, float y);
	void OnTouchUp(unsigned int id, float x, float y);
	void OnTouchMove(unsigned int id, float x, float y);
	void AddToQueue(unsigned int id, float x, float y, TouchEvent::TouchEventState state);
	void ProcessQueue();
	void PostUpdate();
	bool IsPressed(int index) {return touch[index].state == Touch::TouchState::Pressed;}
	bool IsRelease(int index) {return touch[index].state == Touch::TouchState::Released;}
	bool IsDown(int index) {return touch[index].state == Touch::TouchState::Down;}
	bool IsNone(int index) {return touch[index].state == Touch::TouchState::None;}
	Touch GetTouch(int index) {return touch[index];}
};
#endif