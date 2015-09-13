#include "GS_Loading_Template.h"
#include "LoadingManager.h"
#include "Game.h"
void GS_Loading_Template::Init()
{
	iPostInitStep = 0;
}
void GS_Loading_Template::Release()
{
}
void GS_Loading_Template::Update(float timeTotal, float timeDelta)
{
	LoadingManager::GetInstance()->Update();
	if(LoadingManager::GetInstance()->GetTaskCount() == 0)
	{
		if(Game::GetInstance()->GetState(Game::GetInstance()->GetStateCount() - 2)->PostInit(iPostInitStep))
		{
			Game::GetInstance()->PopState();
		}
	}
}
void GS_Loading_Template::Render()
{
	float red[4] = {1,0,0,1};
	float black[4] = {0,0,0,1};
	DXDevice::GetInstance()->ClearColorBuffer(red);
	DXDevice::GetInstance()->ClearDepthStencilBuffer();
	DXDevice::GetInstance()->FlipBuffer();
}