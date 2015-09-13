#ifndef XMATERIAL_H
#define XMATERIAL_H
#include "Shader.h"
#include <vector>
#include <map>

struct Material
{
public:
	Program* GetProgram() { return program; }
	Material()
	{
		program = NULL;
		diffuseTexture = NULL;
		normalTexture = NULL;
	}
	Program* program;
	Texture* diffuseTexture;
	Texture* normalTexture;
};
#endif