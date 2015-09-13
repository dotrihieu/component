#include "Texture.h"
#include "Device_Base.h"
#ifdef USE_DIRECTX
#include "DXDevice.h"
#endif
#include <algorithm>
using namespace std;

//------------------------------------------------------

signed int modifier_table[8][4] =
{
	{-8,  -2,  2,   8 },
	{-17,  -5,  5,  17 },
	{-29,  -9,  9,  29 },
	{-42, -13, 13,  42 },
	{ -60, -18, 18,  60 },
	{-80, -24, 24,  80 },
	{-106, -33, 33, 106 },
	{-183, -47, 47, 183 },
};
uint8_t Read_uint8_t(char **pointer)
{
	uint8_t result;
	memcpy(&result, *pointer, sizeof(uint8_t));
	*pointer += sizeof(uint8_t);
	return result;
}
uint16_t Read_uint16_t(char **pointer)
{
	uint16_t result;
	memcpy(&result, *pointer, sizeof(uint16_t));
	*pointer += sizeof(uint16_t);
	return result;
}
uint32_t Read_uint32_t(char **pointer)
{
	uint32_t result;
	memcpy(&result, *pointer, sizeof(uint32_t));
	*pointer += sizeof(uint32_t);
	return result;
}
unsigned char GetBit(uint64_t value, int index)
{
	return (value >> index) & 0x1;
}
signed char Get3BitSigned(uint64_t value, int index)
{
	if(GetBit(value, index) > 0)
	{
		return ((value >> (index-2)) & 0x7) - 8;
	}
	else
	{
		return (value >> (index-2)) & 0x7;
	}
}
unsigned char Get3Bit(uint64_t value, int index)
{
	return (value >> (index-2)) & 0x7;
}
unsigned char Get4Bit(uint64_t value, int index)
{
	return (value >> (index-3)) & 0xF;
}
unsigned char Get5Bit(uint64_t value, int index)
{
	return (value >> (index-4)) & 0x1F;
}
unsigned char extend_4to8bits(unsigned char value)
{
	return (value << 4) | value;
}
unsigned char extend_5to8bits(unsigned char value)
{
	return (value << 3) | (value >> 2);
}
bool Texture::ReadPVRV2Header(char** pointer)
{
	if(strncmp(*pointer + 44, "PVR!", 4) != 0)
	{
		return false;
	}
	PVRTexHeaderV2 header;
	header.headerLength = Read_uint32_t(pointer);
	header.height = Read_uint32_t(pointer);
	header.width = Read_uint32_t(pointer);
	header.numMipmaps = Read_uint32_t(pointer);
    header.flags = Read_uint32_t(pointer);
    header.dataLength = Read_uint32_t(pointer);
    header.bpp = Read_uint32_t(pointer);
    header.bitmaskRed = Read_uint32_t(pointer);
    header.bitmaskGreen = Read_uint32_t(pointer);
    header.bitmaskBlue = Read_uint32_t(pointer);
    header.bitmaskAlpha = Read_uint32_t(pointer);
    header.pvrTag = Read_uint32_t(pointer);
    header.numSurfs = Read_uint32_t(pointer);
	//--------------------------------------
	this->width = header.width;
	this->height = header.height;
	this->mipmapCount = header.numMipmaps + 1;
	switch(header.flags & 0xFF)
	{
	case 0x36:
		this->format = TEXTURE_FORMAT_ETC;
		break;
	}
	return true;
}
bool Texture::ReadPVRV3Header(char** pointer)
{
	return false;
}
bool Texture::ReadDDSHeader(char** pointer)
{
	return false;
}
bool Texture::ReadKTXHeader(char** pointer)
{
	return false;
}

void Texture::LoadETC(char** pointer)
{
}
bool Texture::LoadTGA(char** pointer)
{
	TGAHeadrer header;
	header.idlength = Read_uint8_t(pointer);
	header.colourmaptype = Read_uint8_t(pointer);
	header.datatypecode = Read_uint8_t(pointer);
	header.colourmaporigin = Read_uint16_t(pointer);
	header.colourmaplength = Read_uint16_t(pointer);
	header.colourmapdepth = Read_uint8_t(pointer);
	header.x_origin = Read_uint16_t(pointer);
	header.y_origin = Read_uint16_t(pointer);
	header.width = Read_uint16_t(pointer);
	header.height = Read_uint16_t(pointer);
	header.bitsperpixel = Read_uint8_t(pointer);
	header.imagedescriptor = Read_uint8_t(pointer);
	if (header.datatypecode == 2 || header.datatypecode == 3)
	{
		if (header.bitsperpixel == 24)
		{
			format = TEXTURE_FORMAT_R8G8B8;
		}
		else if (header.bitsperpixel == 32)
		{
			format = TEXTURE_FORMAT_R8G8B8A8;
		}
		else if (header.bitsperpixel == 8)
		{
			format = TEXTURE_FORMAT_LUMINANCE;
		}
	}
	width = header.width;
	height = header.height;

	unsigned char topToBottom = GetBit(header.imagedescriptor, 5);

	switch(format)
	{
	case TEXTURE_FORMAT_R8G8B8:
		{
			char* buffer = new char[width*height*3];
			for(int y = 0; y < height; y++)
			{
				for(int x = 0; x < width; x++)
				{
					int row = topToBottom ? (height - 1 - y) : y;
					memcpy(buffer + (y*width + x) * 3, *pointer + (row*width + x) * 3 + 2, 1);
					memcpy(buffer + (y*width + x) * 3 + 1, *pointer + (row*width + x) * 3 + 1, 1);
					memcpy(buffer + (y*width + x) * 3 + 2, *pointer + (row*width + x) * 3, 1);
				}
			}
			//glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,buffer);
			Device_Base::GetInstance()->CreateTextureFromMemory(this, buffer);
			delete[] buffer;
			break;
		}
	case TEXTURE_FORMAT_R8G8B8A8:
		{
			char* buffer = new char[width*height * 4];
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int row = topToBottom ? (height - 1 - y) : y;
					memcpy(buffer + (y*width + x) * 4, *pointer + (row*width + x) * 4 + 2, 1);
					memcpy(buffer + (y*width + x) * 4 + 1, *pointer + (row*width + x) * 4 + 1, 1);
					memcpy(buffer + (y*width + x) * 4 + 2, *pointer + (row*width + x) * 4, 1);
					memcpy(buffer + (y*width + x) * 4 + 3, *pointer + (row*width + x) * 4 + 3, 1);
				}
			}
			//glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,buffer);
			Device_Base::GetInstance()->CreateTextureFromMemory(this, buffer);
			delete[] buffer;
			break;
		}
	case TEXTURE_FORMAT_LUMINANCE:
		{
			char* buffer = new char[width*height];
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int row = topToBottom ? (height - 1 - y) : y;
					memcpy(buffer + (y*width + x), *pointer + (row*width + x), 1);
				}
			}

			Device_Base::GetInstance()->CreateTextureFromMemory(this, buffer);
			delete[] buffer;
			break;
		}
	}
	return true;
}
void Texture::DecompressETC(char** pointer)
{
	int decompressDataSize = 0;
	int levelWidth = width;
	int levelHeight = height;
	for (int i = 0; i < mipmapCount; i++) //mipmap count
	{
		decompressDataSize += levelWidth*levelHeight * 3;
		levelWidth = max(levelWidth >> 1, 1);
		levelHeight = max(levelHeight >> 1, 1);
	}
	char* decompressData = new char[decompressDataSize];
	int decompressPointer = 0;
	levelWidth = width;
	levelHeight = height;
	for(int i = 0; i < mipmapCount; i++) //mipmap count
	{
		int workingWidth = max(levelWidth, 4);
		int workingHeight = max(levelHeight, 4);
		char* rawData = new char[workingWidth*workingHeight*3];
		memset(rawData, 0, workingWidth*workingHeight*3);
		for(int row = 0; row < workingHeight/4;row++)
		{
			for(int column = 0;column < workingWidth/4;column++)
			{
				uint64_t block;
				memcpy(&block, *pointer, sizeof(uint64_t));
				*pointer += 8;
				block = _byteswap_uint64(block);
				unsigned char flipbit = GetBit(block, 32);
				unsigned char diffbit = GetBit(block, 33);
				signed int R1, G1, B1, R2, G2, B2;
				unsigned char table_codeword_1, table_codeword_2;
				if (diffbit == 0) //individual mode
				{
					//left block or top block
					R1 =  extend_4to8bits(Get4Bit(block, 63));
					G1 =  extend_4to8bits(Get4Bit(block, 55));
					B1 =  extend_4to8bits(Get4Bit(block, 47));
					//right block or bottom block
					R2 =  extend_4to8bits(Get4Bit(block, 59));
					G2 =  extend_4to8bits(Get4Bit(block, 51));
					B2 =  extend_4to8bits(Get4Bit(block, 43));
				}
				else //differential mode
				{
					//left block or top block
					unsigned char R1a = Get5Bit(block, 63);
					unsigned char G1a = Get5Bit(block, 55);
					unsigned char B1a = Get5Bit(block, 47);

					R1 = extend_5to8bits(R1a);
					G1 = extend_5to8bits(G1a);
					B1 = extend_5to8bits(B1a);
					//right block or bottom block
					signed char dR2 = Get3BitSigned(block, 58);
					signed char dG2 = Get3BitSigned(block, 50);
					signed char dB2 = Get3BitSigned(block, 42);

					R2 = extend_5to8bits(R1a + dR2);
					G2 = extend_5to8bits(G1a + dG2);
					B2 = extend_5to8bits(B1a + dB2);
				}
				table_codeword_1 = Get3Bit(block, 39); //row in table for subblock 1
				table_codeword_2 = Get3Bit(block, 36); //row in table for subblock 1
				if(flipbit == 0) //2x4 side by side
				{
					for(int p = 0; p < 8; p++)
					{
						unsigned char lsb = GetBit(block, p);
						unsigned char msb = GetBit(block, p + 16);
						unsigned char modifyIndex = (((~msb)<<1) & 0x2) | (msb^lsb);
						signed int modifyValue = modifier_table[table_codeword_1][modifyIndex];
						unsigned char R = min(max(R1 + modifyValue, 0), 255);
						unsigned char G = min(max(G1 + modifyValue, 0), 255);
						unsigned char B = min(max(B1 + modifyValue, 0), 255);
						int pixelIndex = row*4*workingWidth + column*4 + (p%4)*workingWidth + p/4;
						rawData[pixelIndex*3] = R;
						rawData[pixelIndex*3 + 1] = G;
						rawData[pixelIndex*3 + 2] = B;
					}
					for(int p = 0; p < 8; p++)
					{
						unsigned char lsb = GetBit(block, p + 8);
						unsigned char msb = GetBit(block, p + 24);
						unsigned char modifyIndex = (((~msb)<<1) & 0x2) | (msb^lsb);
						signed int modifyValue = modifier_table[table_codeword_2][modifyIndex];
						unsigned char R = min(max(R2 + modifyValue, 0), 255);
						unsigned char G = min(max(G2 + modifyValue, 0), 255);
						unsigned char B = min(max(B2 + modifyValue, 0), 255);
						int pixelIndex = row*4*workingWidth + column*4 + (p%4)*workingWidth + p/4 + 2;
						rawData[pixelIndex*3] = R;
						rawData[pixelIndex*3 + 1] = G;
						rawData[pixelIndex*3 + 2] = B;
					}
				}
				else //4x2 top down
				{
					for(int p = 0; p < 8; p++)
					{
						unsigned char lsb = GetBit(block, 2*p - p%2);
						unsigned char msb = GetBit(block, 2*p - p%2 + 16);
						unsigned char modifyIndex = (((~msb)<<1) & 0x2) | (msb^lsb);
						signed int modifyValue = modifier_table[table_codeword_1][modifyIndex];
						unsigned char R = min(max(R1 + modifyValue, 0), 255);
						unsigned char G = min(max(G1 + modifyValue, 0), 255);
						unsigned char B = min(max(B1 + modifyValue, 0), 255);
						int pixelIndex = row*4*workingWidth + column*4 + (p%2)*workingWidth + p/2;
						rawData[pixelIndex*3] = R;
						rawData[pixelIndex*3 + 1] = G;
						rawData[pixelIndex*3 + 2] = B;
					}
					for(int p = 0; p < 8; p++)
					{
						unsigned char lsb = GetBit(block, 2*p - p%2 + 2);
						unsigned char msb = GetBit(block, 2*p - p%2 + 18);
						unsigned char modifyIndex = (((~msb)<<1) & 0x2) | (msb^lsb);
						signed int modifyValue = modifier_table[table_codeword_2][modifyIndex];
						unsigned char R = min(max(R2 + modifyValue, 0), 255);
						unsigned char G = min(max(G2 + modifyValue, 0), 255);
						unsigned char B = min(max(B2 + modifyValue, 0), 255);
						int pixelIndex = (row*4 + 2)*workingWidth + column*4 + (p%2)*workingWidth + p/2;
						rawData[pixelIndex*3] = R;
						rawData[pixelIndex*3 + 1] = G;
						rawData[pixelIndex*3 + 2] = B;
				}
				}
			}
		}
		if(workingWidth > levelWidth || workingHeight > levelHeight)
		{
			char* newData = new char[levelWidth*levelHeight*3];
			for(int y = 0;y<levelHeight;y++)
			{
				memcpy(newData + y*levelWidth*3, rawData + y*workingWidth*3, levelWidth*3);
			}
			//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//glTexImage2D(GL_TEXTURE_2D,i,GL_RGB,levelWidth,levelHeight,0,GL_RGB,GL_UNSIGNED_BYTE,newData);
			memcpy(decompressData + decompressPointer, newData, levelWidth*levelHeight * 3);
			delete[] newData;
		}
		else
		{
			//glTexImage2D(GL_TEXTURE_2D,i,GL_RGB,levelWidth,levelHeight,0,GL_RGB,GL_UNSIGNED_BYTE,rawData);
			memcpy(decompressData + decompressPointer, rawData, levelWidth*levelHeight * 3);
		}
		delete[] rawData;
		decompressPointer += levelWidth*levelHeight * 3;
		levelWidth = levelWidth >> 1;
		levelHeight = levelHeight >> 1;
	}
	Device_Base::GetInstance()->CreateTextureFromMemory(this, decompressData);
	SafeDeleteArray(decompressData);
}

//------------------------------------------------------

Texture* TextureManager::LoadTexture(const char* name)
{
	for(unsigned int i = 0;i<textureList.size();i++)
	{
		if(strcmp(textureList[i]->name, name) == 0)
		{
			return textureList[i];
		}
	}
	char* path = NULL;
	SafeMergeCHAR(&path, GetInstance()->directory, name);
	char* buffer = NULL;
	int length = 0;
	ReadFileToMemory(path, "rt", &buffer, &length);
	FILE *file = fopen(path, "rb");
	if (length > 0)
	{
		Texture* texture = new Texture();
		char* pointer = buffer;

		if (texture->ReadPVRV2Header(&pointer) == true)
		{
#ifdef OS_ANDROID
#else
			texture->format = Texture::TEXTURE_FORMAT_R8G8B8;
			texture->DecompressETC(&pointer);
#endif
		}
		else if (texture->LoadTGA(&pointer) == true)
		{

		}
		else
		{
			SafeDelete(texture);
		}
		if (texture)
		{
			texture->name = new char[strlen(name) + 1];
			strcpy(texture->name, name);
			textureList.push_back(texture);
		}
		SafeDeleteArray(buffer);
		SafeDeleteArray(path);
		return texture;
	}
	else
	{
		SafeDeleteArray(path);
		DebugLog(name);
		return NULL;
	}
}
Texture* TextureManager::GetTexture(const char* name)
{
	for(unsigned int i = 0;i<textureList.size();i++)
	{
		if(strcmp(textureList[i]->name, name) == 0)
		{
			return textureList[i];
		}
	}
	return NULL;

}
Texture* TextureManager::CreateTexture(const char* name, int width, int height, Texture::TextureFormat format, Texture::WrapMode wrap, Texture::FilterMode filter, int aa, unsigned int bindFlags)
{
	for(unsigned int i = 0;i<textureList.size();i++)
	{
		if(strcmp(textureList[i]->name, name) == 0)
		{
			return textureList[i];
		}
	}
	
	Texture* texture = new Texture();
	texture->name = new char[strlen(name) + 1];
	strcpy(texture->name, name);
	texture->width = width;
	texture->height = height;
	texture->format = format;
	texture->wrap = wrap;
	texture->filter = filter;
	Device_Base::GetInstance()->CreateTexture(texture);
	textureList.push_back(texture);
	return texture;
}
void TextureManager::ReleaseTexure(Texture* texture)
{
	textureList.erase(std::remove(textureList.begin(), textureList.end(), texture), textureList.end());
	SafeDelete(texture);
}
void TextureManager::ReleaseTexure(const char* name)
{
	for(std::vector<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
	{
		if(strcmp((*it)->name, name) == 0)
		{
			SafeDelete(*it);
			textureList.erase(it);
			break;
		}
	}
}
Texture::~Texture()
{
	Device_Base::GetInstance()->DeleteTexture(this);
#ifdef USE_DIRECTX
	SafeRelease(shaderResourceView);
	SafeRelease(resource);
#endif
	SafeDeleteArray(name);
}