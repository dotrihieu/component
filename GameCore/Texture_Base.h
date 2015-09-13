#ifndef TEXTURE_BASE_H
#define TEXTURE_BASE_H
#include "Common.h"
#include <vector>

class Texture_Base
{
	friend class TextureManager;
	char* name;
	unsigned int width, height;
	int mipmap;

public:
	enum TextureFormat
	{
		TEXTURE_R8G8B8,
		TEXTURE_R8G8B8A8,
	};
	Texture_Base()
	{
		mipmap = 0;
	}
	virtual ~Texture_Base()
	{
		SafeDeleteArray(name);
	}
	unsigned int GetWidth() {return width;}
	unsigned int GetHeight() {return height;}

	const char* GetName()
	{
		return name;
	}
	bool HasMipmap()
	{
		return mipmap > 0;
	}
};
class TextureManager : public Singleton<TextureManager>
{
	std::vector<Texture_Base*> textureList;
	char* directory;
public:
	static void Init(char* directory = NULL)
	{
		SafeDelete(instance);
		instance = new TextureManager();
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
	~TextureManager()
	{
		for(unsigned int i = 0;i<textureList.size();i++)
		{
			SafeDelete(textureList[i]);
		}
		textureList.clear();
		SafeDeleteArray(directory);
	}
	Texture_Base* LoadTexture(const char* name);
	Texture_Base* GetTexture(const char* name);
	Texture_Base* CreateTexture(const char* name, int width, int height, Texture_Base::TextureFormat format, unsigned int bindFlags, int aa = 0);
	void ReleaseTexure(Texture_Base* texture);
	void ReleaseTexure(const char* name);
	void SetDirectory(char* directory)
	{
		SafeCopyCHAR(&this->directory, directory);
	}
};

#endif