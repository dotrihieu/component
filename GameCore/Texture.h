#ifndef XTEXTURE_H
#define XTEXTURE_H
#include "Common.h"

#ifdef USE_DIRECTX
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\DDSTextureLoader.h"
#include <d3d11_1.h>
#endif
#ifdef WIN32
#include "OpenGLExt.h"
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#include <vector>
#include "TextureFileHeader.h"

/*
OpenGL texcoord is bottom-left. Regular images data store top-left, we need invert data before passing to glTexImage2d
*/

class Texture
{
	friend class TextureManager;

public:
	enum TextureFormat
	{
		TEXTURE_FORMAT_UNKNOWN,
		TEXTURE_FORMAT_R8G8B8,
		TEXTURE_FORMAT_R8G8B8A8,
		TEXTURE_FORMAT_LUMINANCE,
		TEXTURE_FORMAT_ATC,
		TEXTURE_FORMAT_PVR,
		TEXTURE_FORMAT_DXT,
		TEXTURE_FORMAT_ETC,
		TEXTURE_FORMAT_DEPTH24,
		TEXTURE_FORMAT_STENCIL8,
		TEXTURE_FORMAT_DEPTH24_STENCIL8,
	};
	enum WrapMode
	{
#ifdef USE_OPENGL
		WRAP_CLAMP = GL_CLAMP_TO_EDGE,
		WRAP_REPEAR = GL_REPEAT,
		WRAP_MIRROR = GL_MIRRORED_REPEAT,
#else
		WRAP_CLAMP,
		WRAP_REPEAR,
		WRAP_MIRROR,
#endif
	};
	enum FilterMode
	{
#ifdef USE_OPENGL
		FILTER_NEAREST = GL_NEAREST,
		FILTER_LINEAR = GL_LINEAR,
		FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
#else
		FILTER_NEAREST,
		FILTER_LINEAR,
		FILTER_NEARES_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR,
#endif
	};
	#ifdef USE_DIRECTX
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11Resource* resource;
	#endif
	Texture()
	{
		name = NULL;
		mipmapCount = 0;
		bindFlags = 0;
		wrap = WRAP_CLAMP;
		filter = FILTER_LINEAR;
	}
	virtual ~Texture();
	unsigned int GetWidth() {return width;}
	unsigned int GetHeight() {return height;}

	const char* GetName()
	{
		return name;
	}
	bool HasMipmap()
	{
		return mipmapCount > 0;
	}
	//void LoadFromFile(const char* path);
public:
	char* name;
	unsigned int width, height;
	int mipmapCount;
	TextureFormat format;
	FilterMode filter;
	WrapMode wrap;
	unsigned int bindFlags;
#ifdef USE_OPENGL
public:
	unsigned int idGL;
#endif
private:
	bool ReadPVRV2Header(char** pointer);
	bool ReadPVRV3Header(char** pointer);
	bool ReadDDSHeader(char** pointer);
	bool ReadKTXHeader(char** pointer);
	//bool ReadTGAHeader(char** pointer, TGAHeadrer &header);
	void LoadETC(char** pointer);
	void DecompressETC(char** pointer);
	bool LoadTGA(char** pointer);
};
class TextureManager : public Singleton<TextureManager>
{
	std::vector<Texture*> textureList;
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
	TextureManager()
	{
		directory = NULL;
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
	Texture* LoadTexture(const char* name);
	Texture* GetTexture(const char* name);
	Texture* CreateTexture(const char* name, int width, int height, Texture::TextureFormat format, Texture::WrapMode wrap = Texture::WRAP_CLAMP, Texture::FilterMode filter = Texture::FILTER_LINEAR, int aa = 0, unsigned int bindFlags = 0);
	
	void ReleaseTexure(Texture* texture);
	void ReleaseTexure(const char* name);
	void SetDirectory(char* directory)
	{
		SafeCopyCHAR(&this->directory, directory);
	}
};
#ifdef USE_DIRECTX
void GetTextureSize( ID3D11Resource* res, UINT* width, UINT* height );
#endif
#endif