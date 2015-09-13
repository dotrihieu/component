#ifndef XSHADER_H
#define XSHADER_H
#ifdef USE_DIRECTX
#include <d3d11_1.h>
#include "Sampler.h"
#endif
#include <vector>
#include "Common.h"
#include "Texture.h"
#include "Common.h"
#include <vector>

#ifdef USE_DIRECTX
enum UniformID
{
	SU_MATRIX_WORLD,
	SU_MATRIX_VIEW,
	SU_MATRIX_PROJECTION,
	SU_MATRIX_WORLD_VIEW,
	SU_MATRIX_WORLD_VIEW_PROJECTION,
	SU_TEXTURE0,
	SU_SAMPLER0,
	SU_COUNT
};
enum VaryingID
{
	SV_FLOAT3_POSITION,
	SV_FLOAT4_SV_POSITION,
	SV_FLOAT2_TEXCOORD0,
};

struct ShaderDefine
{
	UINT id;
	char* name;
	std::vector<UINT> uniformList;
	std::vector<UINT> varyingList;
	ShaderDefine()
	{
		name  = NULL;
	}
	ShaderDefine(UINT iid, char* iname, std::vector<UINT> iuniform, std::vector<UINT> ivarying)
	{
		name  = NULL;
		id = iid;
		SafeCopyCHAR(&name, iname);
		uniformList = iuniform;
		varyingList = ivarying;
	}
	~ShaderDefine()
	{
		SafeDeleteArray(name);
	}
};

enum ShaderIDList
{
	SIMPLE_VS,
	SIMPLE_PS,
	TEXTURE1_DIFFUSE_VS,
	TEXTURE1_DIFFUSE_PS,
	TEXTURE1_DISTORTION_VS,
	TEXTURE1_DISTORTION_PS,
	SHADER_COUNT
};
struct UniformMap
{
	std::string name;
	UniformID type;
	int offset; //offset in uniformDataStore
	UniformMap(const char* iname, int ioffset)
	{
		name = iname;
		offset = ioffset;
		if(strcmp(iname, "u_world") == 0)
		{
			type = SU_MATRIX_WORLD;
		}
		else if(strcmp(iname, "u_view") == 0)
		{
			type = SU_MATRIX_VIEW;
		}
		else if(strcmp(iname, "u_projection") == 0)
		{
			type = SU_MATRIX_PROJECTION;
		}
		else if(strcmp(iname, "u_world_view") == 0)
		{
			type = SU_MATRIX_WORLD_VIEW;
		}
		else if(strcmp(iname, "u_world_view_projection") == 0)
		{
			type = SU_MATRIX_WORLD_VIEW_PROJECTION;
		}
		else
		{
			type = (UniformID)-1;
		}
	}
};
#endif
class Shader
{
	//friend class ShaderManager;
public:
	char* fileName;
#ifdef USE_DIRECTX
	//std::vector<ShaderUniform> ShaderUniformList;
	std::vector<UniformMap> uniformMap;
	int UniformByTypeOffset[SU_COUNT]; //DirectX
	int sizeOfShaderUniforms;
	//BYTE* uniformDataStore;
	
	ID3D11Buffer* uniformBuffer;
	D3D11_INPUT_ELEMENT_DESC* varyingDescriptionList; //vertex shader only
	ID3D11InputLayout* inputLayout; //vertex shader only
#endif
#ifdef USE_OPENGL
	unsigned int idGL;
#endif
	virtual ~Shader()
	{
		#ifdef USE_DIRECTX
		uniformBuffer->Release();
		SafeDeleteArray(varyingDescriptionList);
		inputLayout->Release();
		#endif
		SafeDeleteArray(fileName);
	}
	const char* GetFileName()
	{
		return fileName;
	}
	virtual void Init(void* ShaderCode, unsigned long ShaderCodeLength, const char* ShaderDesc = NULL, unsigned long ShaderDescLength = 0) = 0;
	bool PushShaderUniformBuffer()
	{
		return true;
	}
	#ifdef USE_DIRECTX
	D3D11_INPUT_ELEMENT_DESC* GetVaryingDescriptionList()
	{
		return varyingDescriptionList;
	}
	ID3D11InputLayout* GetInputLayout()
	{
		return inputLayout;
	}
	#endif
	//bool SetUniformBufferData(int start, int count, void* data);
	//bool SetUniformByType(UINT type, void* data);
};
class VertexShader: public Shader
{
public:
	#ifdef USE_DIRECTX
	ID3D11VertexShader *dxVS;
	#endif
	VertexShader()
	{
#ifdef USE_DIRECTX
		dxVS = NULL;
		sizeOfShaderUniforms = 0;
#endif
	}
	virtual ~VertexShader()
	{
		#ifdef USE_DIRECTX
		SafeRelease(dxVS);
		#endif
	}
	//virtual bool PushShaderUniformBuffer();
	virtual void Init(void* ShaderCode, unsigned long ShaderCodeLength, const char* ShaderDesc = NULL, unsigned long ShaderDescLength = 0);
};
enum TextureID
{
	ST_2D_DIFFUSE,
	ST_COUNT
};
struct TextureMap
{
	std::string name;
	TextureID type;
	int slot;
	TextureMap(const char* iname, int islot)
	{
		name = iname;
		slot = islot;
		if(strcmp(iname, "t_diffuse") == 0)
		{
			type = ST_2D_DIFFUSE;
		}
		else
		{
			type = (TextureID)-1;
		}
	}
};
enum SamplerID
{
	SS_2D_DIFFUSE,
	SS_COUNT
};
struct SamplerMap
{
	std::string name;
	SamplerID type;
	int slot;
	SamplerMap(const char* iname, int islot)
	{
		name = iname;
		slot = islot;
		if(strcmp(iname, "s_diffuse") == 0)
		{
			type = SS_2D_DIFFUSE;
		}
		else
		{
			type = (SamplerID)-1;
		}
	}
};
class PixelShader: public Shader
{
public:
	#ifdef USE_DIRECTX
	ID3D11PixelShader *dxPS;
	#endif
	std::vector<TextureMap> textureMap;
	std::vector<SamplerMap> samplerMap;
	int TextureByTypeSlot[ST_COUNT];
	int SamplerByTypeSlot[SS_COUNT];
	PixelShader()
	{
		#ifdef USE_DIRECTX
		dxPS = NULL;

		sizeOfShaderUniforms = 0;
#endif
	}
	virtual ~PixelShader()
	{
		#ifdef USE_DIRECTX
		SafeRelease(dxPS);
		#endif
	}
	//virtual bool PushShaderUniformBuffer();
	virtual bool SetTexture(Texture* texture, int index = 0);
	#ifdef USE_DIRECTX
	virtual bool SetSampler(Sampler* sampler, int index = 0);
	#endif
	virtual void Init(void* ShaderCode, unsigned long ShaderCodeLength, const char* ShaderDesc = NULL, unsigned long ShaderDescLength = 0);
};
enum ATTRIBUTE_TYPE
{
	ATTRIBUTE_FLOAT3_POSITION,
	ATTRIBUTE_FLOAT2_TEXCOORD_DIFFUSE,
	ATTRIBUTE_FLOAT2_TEXCOORD_LIGHTMAP,
	ATTRIBUTE_FLOAT4_VCOLOR_0,
	ATTRIBUTE_COUNT
};
enum UNIFORM_TYPE
{
	UNIFORM_MATRIX_MODEL,
	UNIFORM_MATRIX_VIEW,
	UNIFORM_MATRIX_PROJECTION,
	UNIFORM_MATRIX_MODEL_VIEW,
	UNIFORM_MATRIX_MODEL_VIEW_PROJECTION,
	UNIFORM_TEXTURE_DIFFUSE,
	UNIFORM_TEXTURE_LIGHTMAP,
	UNIFORM_TEXTURE_DEPTH,
	UNIFORM_FLOAT4_COLOR_AMBIENT,
	UNIFORM_FLOAT2_VIEWPORT_SIZE,
	UNIFORM_COUNT
};

struct AttributeMap
{
	ATTRIBUTE_TYPE type;
	int id;
	AttributeMap()
	{
		type = (ATTRIBUTE_TYPE)-1;
		id = -1;
	}
};
struct UniformMap
{
	UNIFORM_TYPE type;
	int id;
	UniformMap()
	{
		type = (UNIFORM_TYPE)-1;
		id = -1;
	}
};

extern const char* AttributeName[ATTRIBUTE_COUNT];
extern const char* UniformName[UNIFORM_COUNT];
extern const int AttributeElementCount[ATTRIBUTE_COUNT];
extern const int AttributeElementSize[ATTRIBUTE_COUNT];
extern const int AttributeDataType[ATTRIBUTE_COUNT];

class Program
{
public:
	
	
	AttributeMap attributeMap[ATTRIBUTE_COUNT];
	UniformMap uniformMap[UNIFORM_COUNT];
	//std::vector < AttributeMap > attributeMap;
	//std::vector < UniformMap > uniformMap;
	VertexShader* vs;
	PixelShader* ps;
	int idGL;
};

class ShaderManager : public Singleton<ShaderManager>
{
	std::vector<VertexShader*> vsList;
	std::vector<PixelShader*> psList;
	std::vector<Program*> programList;
	char* directory;
public:
	static void Init(const char* directory = NULL);
	static void Release()
	{
		SafeDelete(instance);
	}
	ShaderManager()
	{
		directory = NULL;
	}
	~ShaderManager()
	{
		for (unsigned int i = 0; i<vsList.size(); i++)
		{
			SafeDelete(vsList[i]);
		}
		vsList.clear();
		for (unsigned int i = 0; i<psList.size(); i++)
		{
			SafeDelete(psList[i]);
		}
		psList.clear();
		SafeDeleteArray(directory);
	}

	void AddVertexShader(VertexShader* vs)
	{
		vsList.push_back(vs);
	}
	void AddPixelShader(PixelShader* ps)
	{
		psList.push_back(ps);
	}

	VertexShader*  LoadVertexShaderFromFile(const char* filename);
	PixelShader*  LoadPixelShaderFromFile(const char* filename);
	VertexShader* GetVertexShaderByFileName(const char* filename);
	PixelShader* GetPixelShaderByFileName(const char* filename);
	Program* GetProgramFromFile(const char* vsFileName, const char* psFileName);
	Program* GetProgramFromShader(VertexShader* vs, PixelShader* ps);
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