#ifndef XLOADING_MANAGER
#define XLOADING_MANAGER
#include <vector>
#include "Shader.h"
#include "Common.h"
#include "Sprite.h"
enum LOADING_TASK_TYPE
{
	LOADING_VSHADER,
	LOADING_PSHADER,
	LOADING_TEXTURE,
	LOADING_MESH,
	LOADING_SPRITE,
	LOADING_TEXT,
};
class LoadingTask
{
public:
	LOADING_TASK_TYPE type;
	std::vector<void*> params;
	bool isDone;
	LoadingTask()
	{
		isDone = false;
	}
};
class LoadingManager : public Singleton<LoadingManager>
{
private:
	std::vector<LoadingTask*> loadingTaskList;
public:
	static void Init()
	{
		SafeDelete(instance);
		instance = new LoadingManager();
	}
	int GetTaskCount()
	{
		return loadingTaskList.size();
	}
	void AddLoadingVertexShaderTask(VertexShader* vs);
	void AddLoadingPixelShaderTask(PixelShader* ps);
	void AddLoadingSpriteXMLTask(Sprite* sprite);
	void AddLoadingTextFile(const char* fileName, std::string* result);
	void AddLoadingBinaryFile(const char* fileName, char** result, unsigned int* size);
	void Update();
};
#endif