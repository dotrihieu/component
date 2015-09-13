#include "TouchManager.h"
void TouchManager::OnTouchDown(unsigned int id, float x, float y)
{
	for(int i = 0;i<MAX_TOUCH;i++)
	{
		if(touch[i].id == id)
		{
			touch[i].state = Touch::TouchState::None;
		}
	}
	for(int i = 0;i<MAX_TOUCH;i++)
	{
		if(touch[i].state == Touch::TouchState::None)
		{
			touch[i].id = id;
			touch[i].x = x;
			touch[i].y = y;
			touch[i].state = Touch::TouchState::Pressed;
			break;
		}
	}
}
void TouchManager::OnTouchUp(unsigned int id, float x, float y)
{
	for(int i = 0;i<MAX_TOUCH;i++)
	{
		if(touch[i].id == id)
		{
			touch[i].state = Touch::TouchState::Released;
			break;
		}
	}
}
void TouchManager::OnTouchMove(unsigned int id, float x, float y)
{
	for(int i = 0;i<MAX_TOUCH;i++)
	{
		if(touch[i].id == id)
		{
			touch[i].state = Touch::TouchState::Move;
			break;
		}
	}
}
void TouchManager::AddToQueue(unsigned int id, float x, float y, TouchEvent::TouchEventState state)
{
	TouchEvent touchEvent;
	touchEvent.id = id;
	touchEvent.x = x;
	touchEvent.y = y;
	touchEvent.state = state;
	touchEventQueue.push_back(touchEvent);
}
void TouchManager::PostUpdate()
{
	for(int i = 0;i<MAX_TOUCH;i++)
	{
		touch[i].xPrev = touch[i].x;
		touch[i].yPrev = touch[i].y;
		if(touch[i].state == Touch::TouchState::Pressed || touch[i].state == Touch::TouchState::Move)
		{
			touch[i].state = Touch::TouchState::Down;
		}
		else if(touch[i].state == Touch::TouchState::Released)
		{
			touch[i].state = Touch::TouchState::None;
		}
	}
}
void TouchManager::ProcessQueue()
{
	for(int i = 0;i<touchEventQueue.size();i++)
	{
		if(touchEventQueue[i].state == TouchEvent::TouchEventState::Pressed)
		{
			//find avai touch
			for(int j = 0;j<MAX_TOUCH;j++)
			{
				if(touch[j].state == Touch::TouchState::None)
				{
					touch[j].id = touchEventQueue[i].id;
					touch[j].x = touchEventQueue[i].x;
					touch[j].y = touchEventQueue[i].y;
					touch[j].state = Touch::TouchState::Pressed;
					break;
				}
			}
			touchEventQueue.erase(touchEventQueue.begin()+i);
			i--;
		}
		else if(touchEventQueue[i].state == TouchEvent::TouchEventState::Move)
		{
			bool found = false;
			for(int j = 0;j<MAX_TOUCH;j++)
			{
				if(touch[j].id == touchEventQueue[i].id)
				{
					if(touch[j].state == Touch::TouchState::Down || touch[j].state == Touch::TouchState::Move)
					{
						touch[j].id = touchEventQueue[i].id;
						touch[j].x = touchEventQueue[i].x;
						touch[j].y = touchEventQueue[i].y;
						touch[j].state = Touch::TouchState::Move;
						touchEventQueue.erase(touchEventQueue.begin()+i);
						i--;
					}
					found = true;
					break;
				}
			}
			if(found == false)
			{
				touchEventQueue.erase(touchEventQueue.begin()+i);
				i--;
			}
		}
		else if(touchEventQueue[i].state == TouchEvent::TouchEventState::Released)
		{
			bool found = false;
			for(int j = 0;j<MAX_TOUCH;j++)
			{
				if(touch[j].id == touchEventQueue[i].id)
				{
					if(touch[j].state == Touch::TouchState::Down)
					{
						touch[j].id = touchEventQueue[i].id;
						touch[j].x = touchEventQueue[i].x;
						touch[j].y = touchEventQueue[i].y;
						touch[j].state = Touch::TouchState::Released;
						touchEventQueue.erase(touchEventQueue.begin()+i);
						i--;
					}
					found = true;
					break;
				}
			}
			if(found == false)
			{
				touchEventQueue.erase(touchEventQueue.begin()+i);
				i--;
			}
		}
	}
}