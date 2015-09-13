#include "KeyboardManager.h"

void KeyboardManager::Init()
{
	SafeDelete(instance);
	instance = new KeyboardManager();
	memset(instance->bufferA, 0, KEY_BUFFER_SIZE);
	memset(instance->bufferB, 0, KEY_BUFFER_SIZE);
	instance->newBuffer = instance->bufferA;
	instance->oldBuffer = instance->bufferB;
	instance->AddKeyCodeForUpdate(VK_LEFT);
	instance->AddKeyCodeForUpdate(VK_RIGHT);
}