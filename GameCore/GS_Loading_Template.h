#ifndef XGS_LOADING_TEMPLATE_H
#define XGS_LOADING_TEMPLATE_H
#include "GameState.h"
class GS_Loading_Template: public GameState
{
	virtual void Init();
	virtual void Release();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();
	int iPostInitStep;
};
#endif