#ifndef XGAMESTATE_H
#define XGAMESTATE_H
class GameState
{
public:
	virtual void Init()
	{
	}
	virtual bool PostInit(int& step)
	{
		return true;
	}
	virtual void Release()
	{
	}
	virtual void Update(float timeTotal, float timeDelta)
	{
	}
	virtual void Render()
	{
	}
	virtual void OnTouchDown(unsigned int id, float x, float y)
	{
	}
	virtual void OnTouchUp(unsigned int id, float x, float y)
	{
	}
	virtual void OnTouchMove(unsigned int id, float x, float y)
	{
	}
	virtual void OnKeyDown(int keyCode)
	{
	}
	virtual void OnKeyUp(int keyCode)
	{
	}
};
#endif