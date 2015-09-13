#include "RenderTarget.h"
#include "Device_Base.h"

RenderTarget::RenderTarget(const char* name, int width, int height, bool depth, bool stencil, int aa)
{
	this->name  = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->stencil = stencil;
	this->aa = aa;
	this->colorTexture = NULL;
	this->depthTexture = NULL;
	this->stencilTexture = NULL;
}
RenderTarget::~RenderTarget()
{
	TextureManager::GetInstance()->ReleaseTexure(colorTexture);
	TextureManager::GetInstance()->ReleaseTexure(depthTexture);
	TextureManager::GetInstance()->ReleaseTexure(stencilTexture);
	SafeDeleteArray(name);
}
void RenderTarget::Init()
{
	char* newName = new char[strlen(name) + 7];
	sprintf(newName, "%s_color", name);
	colorTexture = TextureManager::GetInstance()->CreateTexture(newName, width, height
		, Texture::TEXTURE_FORMAT_R8G8B8A8, Texture::WRAP_CLAMP, Texture::FILTER_LINEAR);
	SafeDeleteArray(newName);

	if (colorTexture == NULL)
	{
		return;
	}
	if (depth && stencil 
		&& Device_Base::GetInstance()->IsFeatureSupported(Device_Base::DEVICE_FEATURE_PACKED_DEPTH_STENCIL)
		&& Device_Base::GetInstance()->IsFeatureSupported(Device_Base::DEVICE_FEATURE_DEPTH_TEXTURE))
	{
		newName = new char[strlen(name) + 14];
		sprintf(newName, "%s_depthStencil", name);
		depthTexture = TextureManager::GetInstance()->CreateTexture(newName, width, height
			, Texture::TEXTURE_FORMAT_DEPTH24_STENCIL8, Texture::WRAP_CLAMP, Texture::FILTER_LINEAR);
		stencilTexture = depthTexture;
		SafeDeleteArray(newName);

		if (depthTexture == NULL)
		{
			return;
		}
	}
	if (depth && depthTexture == NULL && Device_Base::GetInstance()->IsFeatureSupported(Device_Base::DEVICE_FEATURE_DEPTH_TEXTURE))
	{
		newName = new char[strlen(name) + 14];
		sprintf(newName, "%s_depth", name);
		depthTexture = TextureManager::GetInstance()->CreateTexture(newName, width, height
			, Texture::TEXTURE_FORMAT_DEPTH24, Texture::WRAP_CLAMP, Texture::FILTER_LINEAR);
		SafeDeleteArray(newName);

		if (depthTexture == NULL)
		{
			return;
		}
	}
	if (stencil && stencilTexture == NULL)
	{
		newName = new char[strlen(name) + 14];
		sprintf(newName, "%s_stencil", name);
		stencilTexture = TextureManager::GetInstance()->CreateTexture(newName, width, height
			, Texture::TEXTURE_FORMAT_STENCIL8, Texture::WRAP_CLAMP, Texture::FILTER_LINEAR);
		SafeDeleteArray(newName);

		if (stencilTexture == NULL)
		{
			return;
		}
	}
	Device_Base::GetInstance()->CreateRenderTarget(this);
}