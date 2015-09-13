#include "Shader.h"
#include "Device_Base.h"
#ifdef USE_DIRECTX
#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
#include <Stringapiset.h>
#include "DXDevice.h"
#endif
#include "LoadingManager.h"
#include "tinyxml2\tinyxml2.h"
//--------------------------------
const char* AttributeName[ATTRIBUTE_COUNT] = { "a_position", "a_texCoord_Diffuse", "a_texCoord_Lightmap"
, "a_vColor0" };
const int AttributeElementCount[ATTRIBUTE_COUNT] = { 3, 2, 2
, 4 };
const int AttributeElementSize[ATTRIBUTE_COUNT] = { sizeof(float), sizeof(float), sizeof(float)
, sizeof(float) };
#ifdef USE_OPENGL
const int AttributeDataType[ATTRIBUTE_COUNT] = { GL_FLOAT, GL_FLOAT, GL_FLOAT
, GL_FLOAT };
#endif
const char* UniformName[UNIFORM_COUNT] = { "u_mMatrix", "u_vMatrix", "u_pMatrix", "u_mvMatrix", "u_mvpMatrix"
, "textureDiffuse", "textureLightmap", "textureDepth", "u_colorAmbient", "u_viewportSize" };
//--------------------------------
void VertexShader::Init(void* ShaderCode, unsigned long ShaderCodeLength, const char* ShaderDesc, unsigned long ShaderDescLength)
{
#ifdef USE_DIRECTX
	for(int i = 0; i < SU_COUNT; i++)
	{
		UniformByTypeOffset[i] = -1;
	}
	if(ShaderDesc)
	{
		tinyxml2::XMLDocument doc;
		doc.Parse(ShaderDesc, ShaderDescLength);
		tinyxml2::XMLElement* inputListNode = doc.FirstChildElement("inputList");
		if(inputListNode)
		{
			int varyingCount =  atoi(inputListNode->Attribute("count"));
			varyingDescriptionList = new D3D11_INPUT_ELEMENT_DESC[varyingCount];
			tinyxml2::XMLElement* inputNode = inputListNode->FirstChildElement("input");
			for(int i = 0; i < varyingCount; i++)
			{
				varyingDescriptionList[i].SemanticName = inputNode->Attribute("semantic");
				varyingDescriptionList[i].SemanticIndex = 0;
				varyingDescriptionList[i].Format = (DXGI_FORMAT)atoi(inputNode->Attribute("dxgi_format"));
				varyingDescriptionList[i].InputSlot = atoi(inputNode->Attribute("index"));
				varyingDescriptionList[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				varyingDescriptionList[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				varyingDescriptionList[i].InstanceDataStepRate = 0;
				inputNode = inputNode->NextSiblingElement();
			}
			DXDevice::GetInstance()->GetD3D11Device()->CreateInputLayout(
							varyingDescriptionList,
							varyingCount,
							ShaderCode,
							ShaderCodeLength,
							&inputLayout
							);
		}

		tinyxml2::XMLElement* uniformBufferListNode = doc.FirstChildElement("constantBufferList");
		if(uniformBufferListNode)
		{
			//Currently we support 1 buffer
			tinyxml2::XMLElement* uniformBufferNode = uniformBufferListNode->FirstChildElement("constantBuffer");
			if(uniformBufferNode)
			{
				sizeOfShaderUniforms = atoi(uniformBufferNode->Attribute("sizeInByte"));
				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.ByteWidth = sizeOfShaderUniforms;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;

				DXDevice::GetInstance()->GetD3D11Device()->CreateBuffer(&bufferDesc, NULL, &uniformBuffer);
				int uniformCount = atoi(uniformBufferNode->Attribute("count"));
				tinyxml2::XMLElement* constantNode = uniformBufferNode->FirstChildElement("variable");
				for(int i = 0; i < uniformCount; i++)
				{
					UniformMap uMap(constantNode->Attribute("name"), atoi(constantNode->Attribute("offsetInBuffer")));
					this->uniformMap.push_back(uMap);
					for(int j = 0; j < SU_COUNT; j++)
					{
						if(uMap.type == j)
						{
							UniformByTypeOffset[j] = uMap.offset;
							break;
						}
					}
					constantNode = constantNode->NextSiblingElement();
				}
			}
		}
	}
	else
	{
		
	}
#endif
}
void PixelShader::Init(void* ShaderCode, unsigned long ShaderCodeLength, const char* ShaderDesc, unsigned long ShaderDescLength)
{
#ifdef USE_DIRECTX
	for(int i = 0; i < SU_COUNT; i++)
	{
		UniformByTypeOffset[i] = -1;
	}
	if(ShaderDesc)
	{
		tinyxml2::XMLDocument doc;
		doc.Parse(ShaderDesc, ShaderDescLength);
		tinyxml2::XMLElement* uniformBufferListNode = doc.FirstChildElement("constantBufferList");
		if(uniformBufferListNode)
		{
			//Currently we support 1 buffer
			tinyxml2::XMLElement* uniformBufferNode = uniformBufferListNode->FirstChildElement("constantBuffer");
			if(uniformBufferNode)
			{
				sizeOfShaderUniforms = atoi(uniformBufferNode->Attribute("sizeInByte"));
				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.ByteWidth = sizeOfShaderUniforms;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;

				DXDevice::GetInstance()->GetD3D11Device()->CreateBuffer(&bufferDesc, NULL, &uniformBuffer);
				int uniformCount = atoi(uniformBufferNode->Attribute("count"));
				tinyxml2::XMLElement* constantNode = uniformBufferNode->FirstChildElement("variable");
				for(int i = 0; i < uniformCount; i++)
				{
					UniformMap uMap(constantNode->Attribute("name"), atoi(constantNode->Attribute("offsetInBuffer")));
					this->uniformMap.push_back(uMap);
					for(int j = 0; j < SU_COUNT; j++)
					{
						if(uMap.type == j)
						{
							UniformByTypeOffset[j] = uMap.offset;
							break;
						}
					}
					constantNode = constantNode->NextSiblingElement();
				}
			}
		}
		tinyxml2::XMLElement* resourceListNode = doc.FirstChildElement("resourceList");
		if(resourceListNode)
		{
			int resourceCount = atoi(resourceListNode->Attribute("count"));
			tinyxml2::XMLElement* resourceNode = resourceListNode->FirstChildElement("resource");
			for(int i = 0; i < resourceCount ; i++)
			{
				D3D_SHADER_INPUT_TYPE type = (D3D_SHADER_INPUT_TYPE)atoi(resourceNode->Attribute("D3D_SHADER_INPUT_TYPE"));
				if(type == D3D_SIT_TEXTURE)
				{
					TextureMap map(resourceNode->Attribute("name"), atoi(resourceNode->Attribute("bindpoint")));
					textureMap.push_back(map);
					for(int j = 0; j < ST_COUNT; j++)
					{
						if(map.type == j)
						{
							TextureByTypeSlot[j] = map.slot;
							break;
						}
					}
				}
				else if(type == D3D_SIT_SAMPLER)
				{
					SamplerMap map(resourceNode->Attribute("name"), atoi(resourceNode->Attribute("bindpoint")));
					samplerMap.push_back(map);
					for(int j = 0; j < SS_COUNT; j++)
					{
						if(map.type == j)
						{
							SamplerByTypeSlot[j] = map.slot;
							break;
						}
					}
				}
				resourceNode = resourceNode->NextSiblingElement();
			}
		}
	}
	else
	{
		
	}
#endif
}
/*
bool Shader::SetUniformBufferData(int start, int count, void* data)
{
	if(start + count > sizeOfShaderUniforms)
	{
		return false;
	}
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;


	// Lock the constant buffer so it can be written to.
	result = DXDevice::GetInstance()->GetDeviceContext()->Map(uniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	void* dataPtr = mappedResource.pData;

	// Copy the matrices into the constant buffer.
	memcpy((BYTE*)dataPtr + start,data,count);

	// Unlock the constant buffer.
	DXDevice::GetInstance()->GetDeviceContext()->Unmap(uniformBuffer, 0);

	return true;
}
*/
/*
bool VertexShader::PushShaderUniformBuffer()
{
	SetUniformBufferData(0, sizeOfShaderUniforms, uniformDataStore);
	DXDevice::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &uniformBuffer);
	return true;
}
bool PixelShader::PushShaderUniformBuffer()
{
	SetUniformBufferData(0, sizeOfShaderUniforms, uniformDataStore);
	DXDevice::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(0, 1, &uniformBuffer);
	return true;
}
*/
bool PixelShader::SetTexture(Texture* texture, int index)
{
#ifdef USE_DIRECTX
	DXDevice::GetInstance()->GetDeviceContext()->PSSetShaderResources(index, 1, &(texture->shaderResourceView));
#endif
	return true;
}
#ifdef USE_DIRECTX
bool PixelShader::SetSampler(Sampler* sampler, int index)
{
	DXDevice::GetInstance()->GetDeviceContext()->PSSetSamplers(index, 1, &(sampler->samplerState));
	return true;
}
#endif
/*
void ShaderManager::CreateVertexShaderFromCompiledResource(char* fileName)
{
	using namespace Windows::Storage;
	using namespace Concurrency;
	wchar_t* wFileName = new wchar_t[strlen(fileName)+1];
	MultiByteToWideChar(CP_UTF8, 0, fileName, -1, wFileName, strlen(fileName)+1);
	Platform::String^ sFileName = ref new Platform::String(wFileName); 
	delete[] wFileName;
	auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
		
	create_task(folder->GetFileAsync(sFileName)).then([] (StorageFile^ file) 
	{
		
		return file->OpenReadAsync();
	}).then([] (Streams::IRandomAccessStreamWithContentType^ stream)
	{
		unsigned int bufferSize = static_cast<unsigned int>(stream->Size);
		auto fileBuffer = ref new Streams::Buffer(bufferSize);
		return stream->ReadAsync(fileBuffer, bufferSize, Streams::InputStreamOptions::None);
	}).then([] (Streams::IBuffer^ fileBuffer)// -> Platform::Array<byte>^ 
	{
		auto fileData = ref new Platform::Array<byte>(fileBuffer->Length);
		Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(fileData);
		VertexShader* vShader = new VertexShader();
		DXDevice::GetInstance()->GetD3D11Device()->CreateVertexShader(
				fileData->Data,
				fileData->Length,
				nullptr,
				&(vShader->dxVS));
		//return fileData;
	});
}
*/
/*
void ShaderManager::LoadVertexShaderFromFile(Platform::String^ filename)
{
	if(GetVertexShaderByFileName(filename) != NULL)
	{
		return;
	}
	else
	{
		VertexShader* vShader = new VertexShader();
		vShader->fileName = filename;
		AddVertexShader(vShader);
		LoadingManager::GetInstance()->AddLoadingVertexShaderTask(vShader);
	}
}
VertexShader* ShaderManager::GetVertexShaderByFileName(Platform::String^ filename)
{
	for(unsigned int i = 0;i<vsList.size();i++)
	{
		if(vsList[i]->GetFileName() == filename)
		{
			return vsList[i];
		}
	}
	return NULL;
}
void ShaderManager::LoadPixelShaderFromFile(Platform::String^ filename)
{
	if(GetPixelShaderByFileName(filename) != NULL)
	{
		return;
	}
	else
	{
		PixelShader* pShader = new PixelShader();
		pShader->fileName = filename;
		AddPixelShader(pShader);
		LoadingManager::GetInstance()->AddLoadingPixelShaderTask(pShader);
	}
}
PixelShader* ShaderManager::GetPixelShaderByFileName(Platform::String^ filename)
{
	for(unsigned int i = 0;i<psList.size();i++)
	{
		if(psList[i]->GetFileName() == filename)
		{
			return psList[i];
		}
	}
	return NULL;
}
*/
void ShaderManager::Init(const char* directory)
{
	SafeDelete(instance);
	instance = new ShaderManager();
	if (directory != NULL)
	{
		SafeCopyCHAR(&instance->directory, directory);
	}
	else
	{
		instance->directory = new char[1];
		instance->directory[0] = 0;
	}
}
VertexShader* ShaderManager::LoadVertexShaderFromFile(const char* filename)
{
	VertexShader* vShader = GetVertexShaderByFileName(filename);
	if(vShader == NULL)
	{
		char* path = NULL;
		SafeMergeCHAR(&path, ShaderManager::GetInstance()->GetDirectory(), filename);
		char* buffer = NULL;
		int length;
		ReadFileToMemory(path, "rb", &buffer, &length);
		if (length > 0)
		{
			vShader = new VertexShader();
			vShader->fileName = new char[strlen(filename) + 1];
			strcpy(vShader->fileName, filename);
			Device_Base::GetInstance()->CreateVertexShaderFromMemory(buffer, length, vShader);
		}
		else
		{
			DebugLog(filename);
		}
		SafeDeleteArray(path);
	}
	return vShader;
}
VertexShader* ShaderManager::GetVertexShaderByFileName(const char* filename)
{
	for(unsigned int i = 0;i<vsList.size();i++)
	{
		if(strcmp(vsList[i]->GetFileName(), filename) == 0)
		{
			return vsList[i];
		}
	}
	return NULL;
}
PixelShader*  ShaderManager::LoadPixelShaderFromFile(const char* filename)
{
	PixelShader* pShader = GetPixelShaderByFileName(filename);
	if(pShader == NULL)
	{
		char* path = NULL;
		SafeMergeCHAR(&path, ShaderManager::GetInstance()->GetDirectory(), filename);
		char* buffer = NULL;
		int length;
		ReadFileToMemory(path, "rb", &buffer, &length);
		if (length > 0)
		{
			pShader = new PixelShader();
			pShader->fileName = new char[strlen(filename) + 1];
			strcpy(pShader->fileName, filename);
			Device_Base::GetInstance()->CreatePixelShaderFromMemory(buffer, length, pShader);
		}
		else
		{
			DebugLog(filename);
		}
		SafeDeleteArray(path);
	}
	return pShader;
}
PixelShader* ShaderManager::GetPixelShaderByFileName(const char* filename)
{
	for(unsigned int i = 0;i<psList.size();i++)
	{
		if(strcmp(psList[i]->GetFileName(), filename) == 0)
		{
			return psList[i];
		}
	}
	return NULL;
}
Program* ShaderManager::GetProgramFromFile(const char* vsFileName, const char* psFileName)
{
	for (unsigned int i = 0; i<programList.size(); i++)
	{
		if (strcmp(programList[i]->vs->GetFileName(), vsFileName) == 0 && strcmp(programList[i]->ps->GetFileName(), psFileName) == 0)
		{
			return programList[i];
		}
	}
	VertexShader* vs = LoadVertexShaderFromFile(vsFileName);
	PixelShader* ps = LoadPixelShaderFromFile(psFileName);
	return GetProgramFromShader(vs, ps);
}
Program* ShaderManager::GetProgramFromShader(VertexShader* vs, PixelShader* ps)
{
	Program* program = new Program();
	program->vs = vs;
	program->ps = ps;
	Device_Base::GetInstance()->CreateProgramFromShader(program);
	programList.push_back(program);
	return program;
}