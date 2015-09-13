#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H
#include "Common.h"
#include <vector>
#include "Windows.h"
#include <vector>
#define KEY_BUFFER_SIZE 256
class KeyboardManager : public Singleton<KeyboardManager>
{
private:
	char bufferA[KEY_BUFFER_SIZE];
	char bufferB[KEY_BUFFER_SIZE];
	char* newBuffer;
	char* oldBuffer;
	float keyUpdateTime[KEY_BUFFER_SIZE];
	std::vector<int> keyCodeNeedTimeUpdate;
public:
	
	static void Init();
	void Update(float timeCurrent)
	{
		char* temp = newBuffer;
		newBuffer = oldBuffer;
		oldBuffer = temp;
		GetKeyboardState((PBYTE)newBuffer);
		for (int i = 0; i < keyCodeNeedTimeUpdate.size(); i++)
		{
			if (IsKeyPressed(keyCodeNeedTimeUpdate[i]))
			{
				keyUpdateTime[keyCodeNeedTimeUpdate[i]] = timeCurrent;
			}
		}
	}
	bool IsKeyPressed(const int keyCode)
	{
		if (HIWORD(oldBuffer[keyCode]) == 0 && HIWORD(newBuffer[keyCode]) == 0xFFFF)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool IsKeyDown(const int keyCode)
	{
		return HIWORD(newBuffer[keyCode]) == 0xFFFF;
	}
	bool IsKeyReleased(const int keyCode)
	{
		if (HIWORD(oldBuffer[keyCode]) == 0xFFFF && HIWORD(newBuffer[keyCode]) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	void AddKeyCodeForUpdate(const int keyCode)
	{
		keyCodeNeedTimeUpdate.push_back(keyCode);
	}
	float GetKeyCodePressedTime(const int keyCode)
	{
		return keyUpdateTime[keyCode];
	}
};
#endif