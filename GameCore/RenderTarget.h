#ifndef XRENDER_TARGET_H
#define XRENDER_TARGET_H
#include "Texture.h"

class RenderTarget
{
	#ifdef USE_DIRECTX
	friend class DXDevice;
	friend class DXDeviceWP8;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	#endif
	Texture* colorTexture;
	Texture* depthTexture;
	Texture* stencilTexture;
	char* name;
	int width;
	int height;
	bool depth;
	bool stencil;
	int aa;
public:
	unsigned int id;
	RenderTarget(const char* name, int width, int height, bool depth = false, bool stencil = false, int aa = 0);
	RenderTarget(const char* name)
	{
		this->name  = new char[strlen(name) + 1];
		strcpy(this->name, name);
	}
	~RenderTarget();
	void Init();
	Texture* GetColorTexture() {return colorTexture;}
	Texture* GetDepthTexture() {return depthTexture;}
	Texture* GetStencilTexture() { return stencilTexture; }
};
#endif