#include "Game.h"
void Game::Update()
{
	timer->Update();
	float timeStart = timer->GetCurrentTime();
	TouchManager::GetInstance()->ProcessQueue();
	KeyboardManager::GetInstance()->Update(timeStart);
	GameState* state = GetCurrentState();
	if(state)
	{
		state->Update(timer->GetTotalAtFrameBegin(), timer->GetDeltaAtFrameBegin());
		//DXDevice::GetInstance()->StartCommandChecking();
		state->Render();
		//DXDevice::GetInstance()->StopCommandChecking();
		Device_Base::GetInstance()->FlipBuffer();
	}
	if(stateToDelete.size() > 0)
	{
		for(int i = stateToDelete.size() - 1;i>=0;i--)
		{
			stateToDelete.back()->Release();
			SafeDelete(stateToDelete.back());
			stateToDelete.pop_back();
		}
	}
	TouchManager::GetInstance()->PostUpdate();
	float timeEnd = timer->GetCurrentTime();
	float timeDelta = timeEnd - timeStart;
	if (timeDelta < 1.0f / 60.0f)
	{
		int timeSleep = (1.0f / 60.0f - timeDelta) * 1000;
		Sleep(timeSleep);
	}
}