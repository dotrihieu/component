#include "XColladaDX.h"
#include "Common.h"
#include "DXDevice.h"
unsigned int ReadUnsignedIntFromBinary(char** buffer)
{
	unsigned int sizeInt = *(unsigned int*)(*buffer);
	*buffer += sizeof(unsigned int);
	return sizeInt;
}
int ReadSignedIntFromBinary(char** buffer)
{
	int sizeInt = *(int*)(*buffer);
	*buffer += sizeof(int);
	return sizeInt;
}
char ReadCharFromBinary(char** buffer)
{
	char sizeChar = *(char*)(*buffer);
	*buffer += sizeof(char);
	return sizeChar;
}
unsigned char ReadUnsignedCharFromBinary(char** buffer)
{
	unsigned char sizeChar = *(unsigned char*)(*buffer);
	*buffer += sizeof(unsigned char);
	return sizeChar;
}
string ReadStringFromBinary(char** buffer)
{
	std::string result;
	unsigned int sizeInt = ReadUnsignedIntFromBinary(buffer);
	result.assign(*buffer, sizeInt);
	*buffer += sizeInt;
	return result;
}
float ReadFloatFromBinary(char** buffer)
{
	float floatValue = *(float*)(*buffer);
	*buffer += sizeof(float);
	return floatValue;
}
void ReadFloatArrayFromBinary(char** buffer, int num, float* output)
{
	memcpy(output, *buffer, sizeof(float) * num);
	*buffer += sizeof(float) * num;
}
void ReadUnsignedIntArrayFromBinary(char** buffer, int num, unsigned int* output)
{
	memcpy(output, *buffer, sizeof(unsigned int) * num);
	*buffer += sizeof(unsigned int) * num;
}
Matrix ReadMatrixFromBinary(char** buffer)
{
	Matrix result;
	for(int i = 0;i<4;i++)
	{
		for(int j = 0;j<4;j++)
		{
			result[i][j] = ReadFloatFromBinary(buffer);
		}
	}
	return result;
}
void XColladaDX::InitFromXCollada(char* buffer)
{
	char* pointer = buffer;
	unsigned int sizeInt;
	unsigned char sizeChar;
	string tag;
	version = ReadStringFromBinary(&pointer);
	tag = ReadStringFromBinary(&pointer);
	unsigned int numberOfNode = ReadUnsignedIntFromBinary(&pointer); //number of nodes
	for(int i = 0; i<numberOfNode; i++)
	{
		XNode* xnode = new XNode();
		xnode->id = ReadStringFromBinary(&pointer);
		xnode->name = ReadStringFromBinary(&pointer);
		tag = ReadStringFromBinary(&pointer);
		xnode->transform = ReadMatrixFromBinary(&pointer);
		tag = ReadStringFromBinary(&pointer);
		unsigned int numberOfGeometry = ReadUnsignedIntFromBinary(&pointer);
		for(int j = 0;j<numberOfGeometry;j++)
		{
			XGeometry* xgeometry = new XGeometry();
			xgeometry->id = ReadStringFromBinary(&pointer);
			xgeometry->name = ReadStringFromBinary(&pointer);
			tag = ReadStringFromBinary(&pointer);
			xgeometry->material = new Material(ReadStringFromBinary(&pointer));
			tag = ReadStringFromBinary(&pointer);
			unsigned char numberOfUniform = ReadUnsignedCharFromBinary(&pointer);
			//unsigned char* temp = new unsigned char[sizeof(float)*16*numberOfUniform];
			int trueSize = 0;
			for(int k = 0;k<numberOfUniform;k++)
			{
				//ShaderUniform* uniform = new ShaderUniform();
				std::string name = ReadStringFromBinary(&pointer);
				unsigned char type = ReadUnsignedCharFromBinary(&pointer);
				int offsetInUniformDataStoreVS = -1;
				int offsetInUniformDataStorePS = -1;
				int slotInTextureList = -1;
				int slotInSamplerList = -1;
				if(type < XColladaNamespace::SUT_Texture2D)
				{
					for(std::vector<UniformMap>::iterator it = xgeometry->material->vs->uniformMap.begin(); it != xgeometry->material->vs->uniformMap.end(); it++)
					{
						if(name.compare((*it).name) == 0)
						{
							offsetInUniformDataStoreVS = (*it).offset;
							break;
						}
					}
					for(std::vector<UniformMap>::iterator it = xgeometry->material->ps->uniformMap.begin(); it != xgeometry->material->ps->uniformMap.end(); it++)
					{
						if(name.compare((*it).name) == 0)
						{
							offsetInUniformDataStorePS = (*it).offset;
							break;
						}
					}
				}
				else if(type == XColladaNamespace::SUT_Texture2D)
				{
					for(std::vector<TextureMap>::iterator it = xgeometry->material->ps->textureMap.begin(); it != xgeometry->material->ps->textureMap.end(); it++)
					{
						if(name.compare((*it).name) == 0)
						{
							slotInTextureList = (*it).slot;
							break;
						}
					}
				}
				else if(type == XColladaNamespace::SUT_SamplerState)
				{
					for(std::vector<SamplerMap>::iterator it = xgeometry->material->ps->samplerMap.begin(); it != xgeometry->material->ps->samplerMap.end(); it++)
					{
						if(name.compare((*it).name) == 0)
						{
							slotInSamplerList = (*it).slot;
							break;
						}
					}
				}
				else
				{
				}
				switch(type)
				{
				case XColladaNamespace::SUT_float:
				case XColladaNamespace::SUT_float2:
				case XColladaNamespace::SUT_float3:
				case XColladaNamespace::SUT_float4:
					for(int t = 0; t < type; t++)
					{
						float f = ReadFloatFromBinary(&pointer);
						if(offsetInUniformDataStoreVS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStoreVS + sizeof(float) * t, &f, sizeof(float));
						}
						if(offsetInUniformDataStorePS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStorePS + sizeof(float) * t, &f, sizeof(float));
						}

					}
					break;
				case XColladaNamespace::SUT_matrix2x2:
					{
						float fArray[4];
						ReadFloatArrayFromBinary(&pointer, 4, fArray);
						if(offsetInUniformDataStoreVS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStoreVS, fArray, sizeof(float) * 4);
						}
						if(offsetInUniformDataStorePS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStorePS, fArray, sizeof(float) * 4);
						}
					}
					break;
				case XColladaNamespace::SUT_matrix3x3:
					{
						float fArray[9];
						ReadFloatArrayFromBinary(&pointer, 9, fArray);
						if(offsetInUniformDataStoreVS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStoreVS, fArray, sizeof(float) * 9);
						}
						if(offsetInUniformDataStorePS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStorePS, fArray, sizeof(float) * 9);
						}
					}
					break;
				case XColladaNamespace::SUT_matrix4x4:
					{
						float fArray[16];
						ReadFloatArrayFromBinary(&pointer, 16, fArray);
						if(offsetInUniformDataStoreVS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStoreVS, fArray, sizeof(float) * 16);
						}
						if(offsetInUniformDataStorePS >= 0)
						{
							memcpy(xgeometry->material->uniformDataStoreVertex + offsetInUniformDataStorePS, fArray, sizeof(float) * 16);
						}
					}
					break;
				case XColladaNamespace::SUT_Texture2D:
					{
						std::string textureName = ReadStringFromBinary(&pointer);
						textureName.append(".DDS");
						if(slotInTextureList >= 0)
						{
							Texture* texture = TextureManager::GetInstance()->LoadTexture(textureName.c_str());
							xgeometry->material->textureList[slotInTextureList] = texture;
						}
					}
					break;
				case XColladaNamespace::SUT_SamplerState:
					/*sizeChar = (*it3).wrapModeU;
					fwrite(&sizeChar, sizeof(unsigned char), 1, file);
					sizeChar = (*it3).wrapModeV;
					fwrite(&sizeChar, sizeof(unsigned char), 1, file);
					sizeChar = (*it3).wrapModeW;
					fwrite(&sizeChar, sizeof(unsigned char), 1, file);
					sizeChar = (*it3).filterMin;
					fwrite(&sizeChar, sizeof(unsigned char), 1, file);
					sizeChar = (*it3).filterMag;
					fwrite(&sizeChar, sizeof(unsigned char), 1, file);*/
					if(slotInSamplerList >= 0)
					{
						D3D11_TEXTURE_ADDRESS_MODE wrapMode = (D3D11_TEXTURE_ADDRESS_MODE)ReadSignedIntFromBinary(&pointer);
						D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
						for(std::vector<TextureMap>::iterator it = xgeometry->material->ps->textureMap.begin(); it != xgeometry->material->ps->textureMap.end(); it++)
						{
							std::string samplerName = (*it).name;
							samplerName.append("Sampler");
							if(name.compare(samplerName) == 0)
							{
								Texture* texture = xgeometry->material->textureList[(*it).slot];
								if(texture->HasMipmap())
								{
									filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
								}
								break;
							}
						}
						Sampler* sampler = new Sampler(filter, wrapMode);
						xgeometry->material->samplerState[slotInSamplerList] = sampler;
					}
					break;
				}
				
			}
			/*
			tag = ReadStringFromBinary(&pointer);
			sizeInt = ReadUnignedIntFromBinary(&pointer);
			xgeometry->indexCount = sizeInt;
			if(sizeInt > 0)
			{
				unsigned int* uiArray = new unsigned int[sizeInt];
				ReadUnsignedIntArrayFromBinary(&pointer, sizeInt, uiArray);
				// Fill in a buffer description.
				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.Usage           = D3D11_USAGE_DEFAULT;
				bufferDesc.ByteWidth       = sizeof( unsigned int ) * sizeInt;
				bufferDesc.BindFlags       = D3D11_BIND_INDEX_BUFFER;
				bufferDesc.CPUAccessFlags  = 0;
				bufferDesc.MiscFlags       = 0;

				// Define the resource data.
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = uiArray;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				// Create the buffer with the device.
				DXDevice::GetInstance()->GetD3D11Device()->CreateBuffer( &bufferDesc, &InitData, &(xgeometry->indexBuffer));

				SafeDeleteArray(uiArray);
			}
			*/
			tag = ReadStringFromBinary(&pointer);
			sizeInt = ReadUnsignedIntFromBinary(&pointer);
			xgeometry->vertexPositionCount = sizeInt/3;
			if(sizeInt > 0)
			{
				float* uiArray = new float[sizeInt];
				ReadFloatArrayFromBinary(&pointer, sizeInt, uiArray);
				D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
				vertexBufferData.pSysMem = uiArray;
				vertexBufferData.SysMemPitch = 0;
				vertexBufferData.SysMemSlicePitch = 0;
				CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(float)*sizeInt, D3D11_BIND_VERTEX_BUFFER);
				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				DXDevice::GetInstance()->GetD3D11Device()->CreateBuffer(
						&vertexBufferDesc,
						&vertexBufferData,
						&(xgeometry->vertexPositionBuffer)
						);
				SafeDeleteArray(uiArray);
			}
			tag = ReadStringFromBinary(&pointer);
			sizeInt = ReadUnsignedIntFromBinary(&pointer);
			xgeometry->vertexTexcoordCount = sizeInt/2;
			if(sizeInt > 0)
			{
				float* uiArray = new float[sizeInt];
				ReadFloatArrayFromBinary(&pointer, sizeInt, uiArray);
				D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
				vertexBufferData.pSysMem = uiArray;
				vertexBufferData.SysMemPitch = 0;
				vertexBufferData.SysMemSlicePitch = 0;
				CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(float)*sizeInt, D3D11_BIND_VERTEX_BUFFER);
				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				DXDevice::GetInstance()->GetD3D11Device()->CreateBuffer(
						&vertexBufferDesc,
						&vertexBufferData,
						&(xgeometry->vertexTexcoordBuffer)
						);
				SafeDeleteArray(uiArray);
			}
			tag = ReadStringFromBinary(&pointer); //normal
			sizeInt = ReadUnsignedIntFromBinary(&pointer);
			xgeometry->vertexTexcoordCount = sizeInt;
			if(sizeInt > 0)
			{
				float* uiArray = new float[sizeInt];
				ReadFloatArrayFromBinary(&pointer, sizeInt, uiArray);
				SafeDeleteArray(uiArray);
			}
			tag = ReadStringFromBinary(&pointer); //tangent
			sizeInt = ReadUnsignedIntFromBinary(&pointer);
			xgeometry->vertexTexcoordCount = sizeInt;
			if(sizeInt > 0)
			{
				float* uiArray = new float[sizeInt];
				ReadFloatArrayFromBinary(&pointer, sizeInt, uiArray);
				SafeDeleteArray(uiArray);
			}
			tag = ReadStringFromBinary(&pointer); //binormal
			sizeInt = ReadUnsignedIntFromBinary(&pointer);
			xgeometry->vertexTexcoordCount = sizeInt;
			if(sizeInt > 0)
			{
				float* uiArray = new float[sizeInt];
				ReadFloatArrayFromBinary(&pointer, sizeInt, uiArray);
				SafeDeleteArray(uiArray);
			}
			//SafeDeleteArray(temp);
			xnode->geometries.push_back(xgeometry);
		}
		this->nodes.push_back(xnode);
	}
}