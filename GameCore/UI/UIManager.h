#ifndef UIMANAGER_H
#define UIMANAGER_H
#include "GameCore\Common.h"
#include <vector>
class UIControl
{
};
class UIManager: public Singleton<UIManager>
{
private:
	std::vector<UIControl*> activeControls;
public:
	void Update(float timeTotal, float timeDelta);
	void Render();
	void OnTouchDown(unsigned int id, float x, float y);
	void OnTouchUp(unsigned int id, float x, float y);
	void OnTouchMove(unsigned int id, float x, float y);
};
#endif