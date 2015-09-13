#ifndef XGAME_H
#define XGAME_H
#ifdef USE_DIRECTX
#include "DXDevice.h"
#endif
#ifdef USE_OPENGL
#include "Device_OpenGL.h"
#endif

#include "LoadingManager.h"
#include "Shader.h"
#include "Common.h"

#include "BasicTimer.h"

#include "GameState.h"
#include "Texture.h"
#include "Sprite.h"
#include "TouchManager.h"
#include "Screen.h"
#include "FontManager.h"
#include "StringManager.h"
#include "KeyboardManager.h"
//huy.vuquang: add sound manager
//#include "SoundEngine/SoundManager.h"
//huy end
#include <vector>
class Game : public Singleton<Game>
{
private:
	BasicTimer* timer;
	std::vector<GameState*> stateStack;
	std::vector<GameState*> stateToDelete;
public:
	~Game()
	{
		StringManager::Release();
		FontManager::Release();
		LoadingManager::Release();
		ShaderManager::Release();
		TextureManager::Release();
		SpriteManager::Release();
		TouchManager::Release();
		KeyboardManager::Release();
	}
	static void Init(int width, int height, void* params[])
	{
		if(instance == NULL)
		{
			instance = new Game();
			instance->timer = new BasicTimer();
			Screen::Init(width, height);
			#ifdef USE_DIRECTX
			Device_Base::Init(Device_Base::DEVICE_WP8_XAML, params);
			#endif
			#ifdef USE_OPENGL
			Device_Base::Init(Device_Base::DEVICE_OPENGL, width, height, params);
			#endif
			LoadingManager::Init();
			ShaderManager::Init("sdcard/");
			TextureManager::Init("sdcard/");
			SpriteManager::Init("sdcard/");
			TouchManager::Init();
			FontManager::Init("sdcard/");
			//SoundManager::Init();
			StringManager::Init("sdcard/string.xml");
			KeyboardManager::Init();
		}
	}
	void OnResume()
	{
		Device_Base::GetInstance()->Resume();
	}
	void OnSuspend()
	{
		Device_Base::GetInstance()->Suspend();
	}

	void PushState(GameState* state)
	{
		stateStack.push_back(state);
		state->Init();
	}
	void PopState()
	{
		if(stateStack.size() > 0)
		{
			stateToDelete.push_back(stateStack.back());
			stateStack.pop_back();
		}
	}
	void SwitchState(GameState* state)
	{
		PopState();
		PushState(state);
	}
	GameState* GetCurrentState()
	{
		if(stateStack.size() > 0)
		{
			return stateStack.back();
		}
		else
		{
			return NULL;
		}
	}
	int GetStateCount()
	{
		return stateStack.size();
	}
	GameState* GetState(int index)
	{
		if(stateStack.size() > index && index >= 0)
		{
			return stateStack[index];
		}
		else
		{
			return NULL;
		}
	}
	void Update();
	void OnTouchDown(int id, float x, float y)
	{
		GameState* state = GetCurrentState();
		if (state)
		{
			state->OnTouchDown(id, x, y);
		}
	}
	void OnKeyDown(int keyCode)
	{
		GameState* state = GetCurrentState();
		if (state)
		{
			state->OnKeyDown(keyCode);
		}
	}
	void OnKeyUp(int keyCode)
	{
		GameState* state = GetCurrentState();
		if (state)
		{
			state->OnKeyUp(keyCode);
		}
	}
};
#endif