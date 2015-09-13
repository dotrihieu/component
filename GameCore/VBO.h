#ifndef VBO_H
#define VBO_H
#include "Shader.h"
#ifdef WIN32
#include "OpenGLExt.h"
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#include <vector>

class VBO
{
public:
	enum VBO_TARGET
	{
		VBO_TARGET_ARRAY_BUFFER,
		VBO_TARGET_ELEMENT_ARRAY_BUFFER,
		VBO_TARGET_COUNT
	};
	enum VBO_USAGE
	{
#ifdef USE_OPENGL
		VBO_USAGE_STATIC_DRAW = GL_STATIC_DRAW,
		VBO_USAGE_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
#else
		VBO_USAGE_STATIC_DRAW,
		VBO_USAGE_DYNAMIC_DRAW,
#endif
	};
	unsigned int idGL;
	VBO_TARGET target;
	VBO_USAGE usage;
	AttributeMap attributeMap[ATTRIBUTE_COUNT];
	int stride;
	int vertexCount;
	int sizeInByte;
	VBO(VBO_TARGET target)
	{
		for (int i = 0; i < ATTRIBUTE_COUNT; i++)
		{
			attributeMap[i].type = (ATTRIBUTE_TYPE)i;
		}
		idGL = 0;
		stride = 0;
		sizeInByte = 0;
		this->target = target;
	}
	void SetAttributeOffset(ATTRIBUTE_TYPE type, int offset)
	{
		attributeMap[type].id = offset;
		stride += AttributeElementCount[type]*AttributeElementSize[type];
		if (sizeInByte > 0)
		{
			vertexCount = sizeInByte / stride;
		}
	}
	~VBO();
};
struct AttributePointer //without VBO
{
	void* pdata;
	int stride;
	AttributePointer()
	{
		pdata = NULL;
		stride = 0;
	}
};
#endif