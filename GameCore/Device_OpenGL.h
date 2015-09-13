#ifndef DEVICE_OPENGL_H
#define DEVICE_OPENGL_H
#include "Device_Base.h"
class Device_OpenGL: public Device_Base
{
public:
	Device_OpenGL();
	virtual void SetActiveVertexShader(VertexShader* vs);
	virtual void SetActivePixelShader(PixelShader* ps);
	virtual void EnableDepthTest();
	virtual void DisableDepthTest();
	virtual void EnableStencilTest();
	virtual void DisableStencilTest();
	virtual void EnableBlend();
	virtual void DisableBlend();
	virtual void EnableFaceCulling();
	virtual void DisableFaceCulling();
	virtual void SetViewport(float x, float y, float width, float height);
	virtual void PushRenderTarget(RenderTarget* rt);
	virtual void PopRenderTarget();
	virtual void Draw(Device_Base::PrimitiveType primitiveType, unsigned int startIndex, unsigned int count);
	virtual void Draw2DTextureScreenCoord(Texture* texture, RectScreenF des, RectScreenF source);
	virtual void Draw2DTextureLogicalCoord(Texture* texture, RectScreenF des, RectScreenF source);
	virtual void Draw2DTextureFullViewport(Texture* texture);
	virtual void CreateVertexShaderFromMemory(char* data, int length, VertexShader* vs);
	virtual void CreatePixelShaderFromMemory(char* data, int length, PixelShader* ps);
	virtual void CreateTexture(Texture *texture);
	virtual void CreateTextureFromMemory(Texture *texture, const char* data);
	virtual void DeleteTexture(Texture *texture);
	virtual void CreateProgramFromShader(Program* program);
	virtual void CreateVBO(VBO* vbo, const char* data, int size);
	virtual void DeleteVBO(VBO* vbo);
	virtual void BindVBO(VBO* vbo);
	virtual void UnbindVBO(VBO::VBO_TARGET target);
	virtual void SetClearColorBuffer(float r, float g, float b, float a);
	virtual void ClearColorDepthStencilBuffer(bool color, bool depth, bool stencil);
	virtual void FlipBuffer();
	virtual void UpdateVertexAttribPointer();
	virtual int BaseToSpecific_VBO_TARGET(VBO::VBO_TARGET target);
	virtual void UseProgram(Program* prg);
	virtual void SetUniformTexture(UNIFORM_TYPE type, Texture* texture);
	virtual void SetUniformMatrix(UNIFORM_TYPE type, Matrix &matrix);
	virtual void SetUniformVec4f(UNIFORM_TYPE type, Vector4f &vertex);
	virtual void SetUniformVec2f(UNIFORM_TYPE type, const float &v1, const float &v2);
	virtual void DrawArray(PrimitiveType type, int offset, int count);
	virtual void DrawElement(PrimitiveType type, int count, const void* indices);
	virtual void SetBlendState(const BlendState &blendState);
	virtual void CreateRenderTarget(RenderTarget* rt);
	virtual void SetPolygonMode(PolygonMode mode);
};
#endif