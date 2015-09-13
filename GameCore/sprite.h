#ifndef XSPRITE_H
#define XSPRITE_H
#include "Common.h"
#include <vector>
#include "XMath.h"
#include "Texture.h"
#include "Bound.h"
#ifdef USE_DIRECTX
#include "Sampler.h"
#endif
#include "VBO.h"

struct SpriteModule
{
	int id;
	int width, height;
	RectScreenF texcoord;
};
struct SpriteFrameModule
{
	SpriteModule* module;
	int x, y, width, height;
	bool flipH, flipV;
};
struct SpriteFrame
{
	int id;
	std::vector<SpriteFrameModule*> frameModuleList;
	BoundGroup* boundGroups[BoundGroup::BG_TYPE_COUNT];
	BoundGroup* boundGroupsLogical[BoundGroup::BG_TYPE_COUNT];
	int indexInVBO;
	int countInVBO;
	std::string desc;
	SpriteFrame()
	{
		id = 0;
		indexInVBO = 0;
		countInVBO = 0;
		for(int i = 0;i<BoundGroup::BG_TYPE_COUNT;i++)
		{
			boundGroups[i] = new BoundGroup();
			boundGroupsLogical[i] = new BoundGroup();
		}
	}
	~SpriteFrame()
	{
		for(int i = 0;i<BoundGroup::BG_TYPE_COUNT;i++)
		{
			SafeDelete(boundGroups[i]);
		}
	}
};
struct SpriteAnimFrame
{
	float delay;
	SpriteFrame* frame;
};
struct SpriteAnim
{
	int id;
	bool loop;
	std::string desc;
	std::vector<SpriteAnimFrame*> animFrameList;
};
struct SpriteAnimInstance
{
	SpriteAnim* anim;
	float time;
	int currentFrame;
	bool finished;
	void Update(float dt);
	bool IsFinished();
	SpriteAnimInstance()
	{
		finished = false;
	}
	void Play()
	{
		currentFrame = 0;
		time = 0.0f;
		finished = false;
	}
};
class Sprite
{
	friend class SpriteManager;
	char* fileName;
	std::vector<SpriteModule*> moduleList;
	std::vector<SpriteFrame*> frameList;
	std::vector<SpriteAnim*> animList;
	Texture* texture;
	VBO* vbo;
#ifdef USE_DIRECTX
	Sampler* sampler;
#endif
public:
	Sprite()
	{
		fileName = NULL;
		#ifdef USE_DIRECTX
		sampler = new Sampler(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
		#endif
	}
	virtual ~Sprite()
	{
		SafeDeleteArray(fileName);
		SafeDelete(vbo);
		#ifdef USE_DIRECTX
		SafeDelete(sampler);
		#endif
	}
	const char* GetFileName()
	{
		return fileName;
	}
	void InitFromXML(const char* buffer);
	void DrawFrame(int id, float x, float y);
	void DrawFrameLogical(int id, float x, float y);
	void DrawModule(int id, float x, float y);
	void DrawModuleLogical(int id, float x, float y);
	void DrawAnim(SpriteAnimInstance* animInstance, float x, float y);
	void DrawAnimLogical(SpriteAnimInstance* animInstance, float x, float y);

	SpriteFrame* GetFrame(int index)
	{
		return frameList[index];
	}
	int GetFrameCount()
	{
		return frameList.size();
	}
	int GetModuleCount()
	{
		return moduleList.size();
	}
	SpriteModule* GetModule(int index)
	{
		return moduleList[index];
	}
	SpriteAnim* GetAnimByIndex(int index);
	SpriteAnim* GetAnimByID(int id);
	SpriteAnim* GetAnimByDesc(const char* desc);
};
class SpriteManager : public Singleton<SpriteManager>
{
	std::vector<Sprite*> spriteList;
	char* directory;
public:
	SpriteManager()
	{
		directory = NULL;
	}
	~SpriteManager()
	{
		for(int i = 0;i<spriteList.size();i++)
		{
			SafeDelete(spriteList[i]);
		}
		spriteList.clear();
		SafeDeleteArray(directory);
	}
	static void Init(char* directory = NULL)
	{
		SafeDelete(instance);
		instance = new SpriteManager();
		if(directory != NULL)
		{
			SafeCopyCHAR(&instance->directory, directory);
		}
		else
		{
			instance->directory = new char[1];
			instance->directory[0] = 0;
		}
	}
	static void Release()
	{
		SafeDelete(instance);
	}
	Sprite* GetSpriteByFileName(char* filename);
	Sprite* LoadSpriteFromXMLFile(char* filename);
	void SetDirectory(char* directory)
	{
		SafeCopyCHAR(&this->directory, directory);
	}
	const char* GetDirectory()
	{
		return directory;
	}
};
#endif