#include "Device_OpenGL.h"
#ifdef WIN32
#include "OpenGLExt.h"
#include "GameCore/Device_OpenGL.h"
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef WIN32
extern HDC hdc;
#endif
#include "Screen.h"
Device_OpenGL::Device_OpenGL()
{
	for (int i = 0; i < DEVICE_FEATURE_COUNT; i++)
	{
		deviceFeatureSupport[i] = 0;
	}
}
void Device_OpenGL::SetActiveVertexShader(VertexShader* vs)
{
}
void Device_OpenGL::SetActivePixelShader(PixelShader* ps)
{
}
void Device_OpenGL::EnableDepthTest()
{
	glEnable(GL_DEPTH_TEST);
}
void Device_OpenGL::DisableDepthTest()
{
	glDisable(GL_DEPTH_TEST);
}
void Device_OpenGL::EnableStencilTest()
{
	glEnable(GL_STENCIL_TEST);
}
void Device_OpenGL::DisableStencilTest()
{
	glDisable(GL_STENCIL_TEST);
}
void Device_OpenGL::EnableBlend()
{
	glEnable(GL_BLEND);
}
void Device_OpenGL::DisableBlend()
{
	glDisable(GL_BLEND);
}
void Device_OpenGL::EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}
void Device_OpenGL::DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}
void Device_OpenGL::SetViewport(float x, float y, float width, float height)
{
	glViewport(x, y, width, height);
}
void Device_OpenGL::PushRenderTarget(RenderTarget* rt)
{
	renderTargetStack.push_back(rt);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->id);
}
void Device_OpenGL::PopRenderTarget()
{
	if (renderTargetStack.size() > 0)
	{
		renderTargetStack.pop_back();
	}
	if (renderTargetStack.size() > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GetCurrentRenderTarget()->id);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void Device_OpenGL::Draw(Device_Base::PrimitiveType primitiveType, unsigned int startIndex, unsigned int count)
{
}
void Device_OpenGL::Draw2DTextureScreenCoord(Texture* texture, RectScreenF des, RectScreenF source)
{
}
void Device_OpenGL::Draw2DTextureLogicalCoord(Texture* texture, RectScreenF des, RectScreenF source)
{
}
void Device_OpenGL::Draw2DTextureFullViewport(Texture* texture)
{
	DisableDepthTest();
	PushMatrix();
	SetIdentityMatrix();
	UnbindVBO(VBO::VBO_TARGET_ARRAY_BUFFER);
	//-------------------------------------------
	float x = Screen::GetInstance()->GetRatio();
	float y = 1.0f;
	float z = -1.0f;
	float vertices[30] = { -x, y, z, 0.0f, 1.0f,
							-x, -y, z, 0.0f, 0.0f,
							x, -y, z, 1.0f, 0.0f,
							-x, y, z, 0.0f, 1.0f,
							x, -y, z, 1.0f, 0.0f,
							x, y, z, 1.0f, 1.0f
	};
	SetAttributePointer(ATTRIBUTE_FLOAT3_POSITION, vertices, sizeof(float) * 5);
	SetAttributePointer(ATTRIBUTE_FLOAT2_TEXCOORD_DIFFUSE, vertices + 3, sizeof(float) * 5);
	Device_Base::GetInstance()->UpdateVertexAttribPointer();
	Device_Base::GetInstance()->SetUniformTexture(UNIFORM_TEXTURE_DIFFUSE, texture);
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, m_MatrixProjection2DLogical);

	Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, 0, 6);
	//-------------------------------------------
	EnableDepthTest();
	PopMatrix();
	
}
void Device_OpenGL::CreateVertexShaderFromMemory(char* data, int length, VertexShader* vs)
{
	vs->idGL = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs->idGL, 1, &data, &length);
	glCompileShader(vs->idGL);
	GLsizei size;
	char log[1024];
	glGetShaderInfoLog(vs->idGL, 1024, &size, log);
	if (size > 0)
	{
		DebugLog(log);
	}
}
void Device_OpenGL::CreatePixelShaderFromMemory(char* data, int length, PixelShader* ps)
{
	ps->idGL = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ps->idGL, 1, &data, &length);
	glCompileShader(ps->idGL);
	GLsizei size;
	char log[1024];
	glGetShaderInfoLog(ps->idGL, 1024, &size, log);
	if (size > 0)
	{
		DebugLog(log);
	}
}
void Device_OpenGL::CreateTexture(Texture *texture)
{
	CreateTextureFromMemory(texture, NULL);
}
void Device_OpenGL::CreateTextureFromMemory(Texture *texture, const char* data)
{
	int internalformat = 0;
	int formatTexel = 0;
	int type = 0;
	bool compressed = false;
	int sizeInByte = 0;
	switch (texture->format)
	{
	case Texture::TEXTURE_FORMAT_R8G8B8:
		internalformat = GL_RGB8;
		formatTexel = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		compressed = false;
		break;
	case Texture::TEXTURE_FORMAT_R8G8B8A8:
		internalformat = GL_RGBA8;
		formatTexel = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		compressed = false;
		break;
	case Texture::TEXTURE_FORMAT_LUMINANCE:
		internalformat = GL_LUMINANCE;
		formatTexel = GL_LUMINANCE;
		type = GL_UNSIGNED_BYTE;
		compressed = false;
		break;
	case Texture::TEXTURE_FORMAT_ETC:
		compressed = true;
		break;
	case Texture::TEXTURE_FORMAT_DEPTH24_STENCIL8:
		internalformat = GL_RGBA8;
		formatTexel = GL_DEPTH_STENCIL;
		type = GL_UNSIGNED_INT_24_8;
		compressed = false;
		break;
	}
	int minFilter = GL_LINEAR;
	if (texture->filter == GL_LINEAR || texture->filter == GL_LINEAR_MIPMAP_LINEAR || texture->filter == GL_LINEAR_MIPMAP_NEAREST)
	{
		minFilter = GL_LINEAR;
	}
	else
	{
		minFilter = GL_NEAREST;
	}
	glGenTextures(1, &(texture->idGL));
	int currentTextureBinded = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureBinded);
	glBindTexture(GL_TEXTURE_2D, texture->idGL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filter);
	//-----------------------------------
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//-----------------------------------
	if (compressed == false)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture->width, texture->height, 0, formatTexel, type, data);
	}
	else
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture->width, texture->height, 0, sizeInByte, data);
	}
	glBindTexture(GL_TEXTURE_2D, currentTextureBinded);
}
void Device_OpenGL::DeleteTexture(Texture *texture)
{
	glDeleteTextures(1, &(texture->idGL));
}
void Device_OpenGL::CreateProgramFromShader(Program* program)
{
	program->idGL = glCreateProgram();
	glAttachShader(program->idGL, program->vs->idGL);
	glAttachShader(program->idGL, program->ps->idGL);
	glLinkProgram(program->idGL);
	GLsizei size;
	char log[1024];
	glGetProgramInfoLog(program->idGL, 1024, &size, log);
	if (size > 0)
	{
		DebugLog(log);
	}
	//---------------------
	
	for (int i = 0; i < ATTRIBUTE_COUNT; i++)
	{
		program->attributeMap[i].type = (ATTRIBUTE_TYPE)i;
		program->attributeMap[i].id = glGetAttribLocation(program->idGL, AttributeName[i]);
		/*
		int id = glGetAttribLocation(program->idGL, AttributeName[i]);
		if (id >= 0)
		{
			AttributeMap att;
			att.id = id;
			att.type = (ATTRIBUTE_TYPE)i;
			program->attributeMap.push_back(att);
		}
		*/
	}
	for (int i = 0; i < UNIFORM_COUNT; i++)
	{
		program->uniformMap[i].type = (UNIFORM_TYPE)i;
		program->uniformMap[i].id = glGetUniformLocation(program->idGL, UniformName[i]);
		/*
		int id = glGetUniformLocation(program->idGL, UniformName[i]);
		if (id >= 0)
		{
			UniformMap uni;
			uni.id = id;
			uni.type = (UNIFORM_TYPE)i;
			program->uniformMap.push_back(uni);
		}
		*/
	}
	//---------------------
}

void Device_OpenGL::CreateVBO(VBO* vbo, const char* data, int size)
{
	GLenum target = BaseToSpecific_VBO_TARGET(vbo->target);
	glGenBuffers(1, &(vbo->idGL));
	int oldBufferBinded;
	glGetIntegerv(target, &oldBufferBinded);
	glBindBuffer(target, vbo->idGL);
	glBufferData(target, size, data, vbo->usage);
	glBindBuffer(target, oldBufferBinded);
	vbo->sizeInByte = size;
	if (vbo->target == VBO::VBO_TARGET_ELEMENT_ARRAY_BUFFER)
	{
		vbo->vertexCount = vbo->sizeInByte / sizeof(unsigned int);
	}
	else if (vbo->stride > 0)
	{
		vbo->vertexCount = vbo->sizeInByte / vbo->stride;
	}
}
void Device_OpenGL::DeleteVBO(VBO* vbo)
{
	glDeleteBuffers(1, &(vbo->idGL));
}
void Device_OpenGL::BindVBO(VBO* vbo)
{
	GLenum target = BaseToSpecific_VBO_TARGET(vbo->target);
	glBindBuffer(target, vbo->idGL);
	vboActivated[vbo->target] = vbo;
}
void Device_OpenGL::UpdateVertexAttribPointer()
{
	if (programActivated == NULL)
	{
		return;
	}
	if (vboActivated[VBO::VBO_TARGET_ARRAY_BUFFER] == NULL)
	{
		for (int i = 0; i < ATTRIBUTE_COUNT; i++)
		{
			if (attributePointerActivated[i].pdata != NULL
				&& programActivated->attributeMap[i].id >= 0)
			{
				glEnableVertexAttribArray(programActivated->attributeMap[i].id);
				glVertexAttribPointer(programActivated->attributeMap[i].id, AttributeElementCount[i], AttributeDataType[i]
					, GL_FALSE, attributePointerActivated[i].stride, (GLvoid*)attributePointerActivated[i].pdata);
			}
		}
	}
	else
	{
		VBO* vbo = vboActivated[VBO::VBO_TARGET_ARRAY_BUFFER];
		for (int i = 0; i < ATTRIBUTE_COUNT; i++)
		{
			if (vbo->attributeMap[i].id >= 0
				&& programActivated->attributeMap[i].id >= 0)
			{
				glEnableVertexAttribArray(programActivated->attributeMap[i].id);
				glVertexAttribPointer(programActivated->attributeMap[i].id, AttributeElementCount[i], AttributeDataType[i], GL_FALSE, vbo->stride, (GLvoid*)vbo->attributeMap[i].id);
			}
		}
	}
}
void Device_OpenGL::UnbindVBO(VBO::VBO_TARGET target)
{
	GLenum targetSpec = BaseToSpecific_VBO_TARGET(target);
	glBindBuffer(targetSpec, 0);
	vboActivated[target] = NULL;
}
void Device_OpenGL::SetClearColorBuffer(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}
void Device_OpenGL::ClearColorDepthStencilBuffer(bool color, bool depth, bool stencil)
{
	GLbitfield mask = 0;
	if (color)
	{
		mask = mask | GL_COLOR_BUFFER_BIT;
	}
	if (depth)
	{
		mask = mask | GL_DEPTH_BUFFER_BIT;
	}
	if (stencil)
	{
		mask = mask | GL_STENCIL_BUFFER_BIT;
	}
	glClear(mask);
}

void Device_OpenGL::FlipBuffer()
{
#ifdef WIN32
	SwapBuffers(hdc);
#endif
#ifdef OS_ANDROID
	eglSwapBuffer();
#endif
}
int Device_OpenGL::BaseToSpecific_VBO_TARGET(VBO::VBO_TARGET target)
{
	int targetSpec;
	if (target == VBO::VBO_TARGET_ARRAY_BUFFER)
	{
		targetSpec = GL_ARRAY_BUFFER;
	}
	else if (target == VBO::VBO_TARGET_ELEMENT_ARRAY_BUFFER)
	{
		targetSpec = GL_ELEMENT_ARRAY_BUFFER;
	}
	return targetSpec;
}
void Device_OpenGL::UseProgram(Program* prg)
{
	if (prg)
	{
		glUseProgram(prg->idGL);
	}
	else
	{
		glUseProgram(0);
	}
	programActivated = prg;
}
void Device_OpenGL::SetUniformTexture(UNIFORM_TYPE type, Texture* texture)
{
	if (texture == NULL || programActivated == NULL)
	{
		return;
	}
	int slotToActive = 0;
	if (textureSlotActivated[type] >= 0)
	{
		slotToActive = textureSlotActivated[type];
	}
	else
	{
		slotToActive = numTextureActivated;
		numTextureActivated++;
	}
	textureSlotActivated[type] = slotToActive;
	glActiveTexture(GL_TEXTURE0 + slotToActive);
	glBindTexture(GL_TEXTURE_2D, texture->idGL);
	glUniform1i(programActivated->uniformMap[type].id, slotToActive);
}
void Device_OpenGL::SetUniformMatrix(UNIFORM_TYPE type, Matrix &matrix)
{
	if (programActivated == NULL)
	{
		return;
	}
	glUniformMatrix4fv(programActivated->uniformMap[type].id, 1, GL_FALSE, matrix);
}
void Device_OpenGL::SetUniformVec4f(UNIFORM_TYPE type, Vector4f &vertex)
{
	if (programActivated == NULL)
	{
		return;
	}
	glUniform4f(programActivated->uniformMap[type].id, vertex.x, vertex.y, vertex.z, vertex.w);
}
void Device_OpenGL::SetUniformVec2f(UNIFORM_TYPE type, const float &v1, const float &v2)
{
	if (programActivated == NULL)
	{
		return;
	}
	glUniform2f(programActivated->uniformMap[type].id, v1, v2);
}
void Device_OpenGL::DrawArray(PrimitiveType type, int offset, int count)
{
	glDrawArrays(type, offset, count);
}
void Device_OpenGL::DrawElement(PrimitiveType type, int count, const void* indices)
{
	glDrawElements(type, count, GL_UNSIGNED_INT, indices);
}
void Device_OpenGL::SetBlendState(const BlendState &blendState)
{
	currentBlendState = blendState;
	glBlendEquationSeparate(blendState.colorBlendEquation, blendState.alphaBlendEquation);
	glBlendFuncSeparate(blendState.colorSrcBlendFactor, blendState.colorDestBlendFactor, blendState.alphaSrcBlendFactor, blendState.alphaDestBlendFactor);
}
void Device_OpenGL::CreateRenderTarget(RenderTarget* rt)
{
	glGenFramebuffers(1, &(rt->id));
	glBindFramebuffer(GL_FRAMEBUFFER, rt->id);
	//-------------------------------------
	if (rt->GetColorTexture())
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->GetColorTexture()->idGL, 0);
	}
	//-------------------------------------
	if (rt->GetDepthTexture())
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rt->GetDepthTexture()->idGL, 0);
	}
	if (rt->GetStencilTexture())
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rt->GetStencilTexture()->idGL, 0);
	}
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		DebugLog("FBO error");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Device_OpenGL::SetPolygonMode(PolygonMode mode)
{
	glPolygonMode(GL_FRONT, mode);
	glPolygonMode(GL_BACK, mode);
}