#include "FontManager.h"
#include "tinyxml2\tinyxml2.h"
#include "Screen.h"
#include "Device_Base.h"

float FontManager::fontScaleGlobal = 0.005f;

Font* FontManager::LoadFontFromXML(const char* buffer)
{
	Font* font = new Font();
	tinyxml2::XMLDocument doc;
	doc.Parse( buffer );
	if(doc.ErrorID() != 0)
	{
		return NULL;
	}
	tinyxml2::XMLElement* root = doc.FirstChildElement("font");
	tinyxml2::XMLElement* common = root->FirstChildElement("common");
	font->lineHeight = common->IntAttribute("lineHeight");
	font->textureOrginalWidth = common->IntAttribute("scaleW");
	font->textureOrginalHeight = common->IntAttribute("scaleH");
	tinyxml2::XMLElement* pages = root->FirstChildElement("pages");
	tinyxml2::XMLElement* page = pages->FirstChildElement("page");
	std::string imageName = page->Attribute("file");
	font->texture = TextureManager::GetInstance()->LoadTexture(imageName.c_str());
	tinyxml2::XMLElement* chars = root->FirstChildElement("chars");
	int count = chars->IntAttribute("count");
	tinyxml2::XMLElement* charXML = chars->FirstChildElement("char");
	while (charXML)
	{
		FontCharacter fontChar;
		fontChar.texcoordLeft = float(charXML->IntAttribute("x"));
		fontChar.texcoordTop = float(charXML->IntAttribute("y"));
		fontChar.width = charXML->IntAttribute("width");
		fontChar.height = charXML->IntAttribute("height");
		fontChar.texcoordRight = fontChar.texcoordLeft + fontChar.width;
		fontChar.texcoordBottom = fontChar.texcoordTop + fontChar.height;
		fontChar.texcoordLeft /= (float)font->textureOrginalWidth;
		fontChar.texcoordTop = 1.0f - fontChar.texcoordTop / (float)font->textureOrginalHeight;
		fontChar.texcoordRight /= (float)font->textureOrginalWidth;
		fontChar.texcoordBottom = 1.0f - fontChar.texcoordBottom / (float)font->textureOrginalHeight;
		fontChar.xoffset = charXML->IntAttribute("xoffset");
		fontChar.yoffset = -charXML->IntAttribute("yoffset");
		fontChar.xadvance = charXML->IntAttribute("xadvance");
		int id = charXML->IntAttribute("id");
		font->characterMap[id] = fontChar;
		charXML = charXML->NextSiblingElement("char");
	}
	tinyxml2::XMLElement* kernings = root->FirstChildElement("kernings");
	tinyxml2::XMLElement* kerningXML = kernings->FirstChildElement("kerning");
	while (kerningXML)
	{
		long long  first = kerningXML->IntAttribute("first");
		long long  second = kerningXML->IntAttribute("second");
		int amount = kerningXML->IntAttribute("amount");
		long long key = (first << 32) | second;
		font->kerningMap[key] = amount;
		kerningXML = kerningXML->NextSiblingElement("kerning");
	}
	return font;
}

void FontManager::Init(const char* directory)
{
	SafeDelete(instance);
	instance = new FontManager();
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

Font* FontManager::GetFontByFileName(const char* filename)
{
	for (unsigned int i = 0; i<fontList.size(); i++)
	{
		if (strcmp(fontList[i]->fileName, filename) == 0)
		{
			return fontList[i];
		}
	}
	return NULL;
}

Font* FontManager::LoadFontFromFile(const char* filename)
{
	Font* font = GetFontByFileName(filename);
	if (font != NULL)
	{
		return font;
	}
	else
	{
		char* path = NULL;
		SafeMergeCHAR(&path, GetInstance()->directory, filename);
		char* buffer = NULL;
		int length = 0;
		ReadFileToMemory(path, "rt", &buffer, &length);
		if (length > 0)
		{
			font = LoadFontFromXML(buffer);
			SafeCopyCHAR(&(font->fileName), filename);
			fontList.push_back(font);
			SafeDeleteArray(buffer);
		}
		SafeDeleteArray(path);
		return font;
	}
}

void FontManager::DrawASCII(const float &xIn, const float &yIn, Font* font, const char* text) //Heavy call, only for debug
{
	int xPointer = 0;
	int yPointer = 0;
	float z = -1.0f;
	int length = strlen(text);
	float* buffer = new float[length * 6 * 5];
	float* pointer = buffer;
	int validCharacter = 0;
	long long previousCharacter = 0;
	for (int i = 0; i < length; i++)
	{
		if (text[i] == '\n')
		{
			xPointer = 0;
			yPointer -= font->lineHeight;
			continue;
		}
		std::map<int, FontCharacter>::iterator it = font->characterMap.find((int)text[i]);
		if (it != font->characterMap.end())
		{
			if (previousCharacter != 0)
			{
				long long  second = text[i];
				long long key = (previousCharacter << 32) | second;
				std::map<long long, int>::iterator it2 = font->kerningMap.find(key);
				if (it2 != font->kerningMap.end())
				{
					xPointer += it2->second;
				}
			}
			int x = xPointer + it->second.xoffset;
			int y = yPointer + it->second.yoffset;
			pointer[0] = x;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			pointer[0] = x;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			xPointer += it->second.xadvance;
			validCharacter++;
			previousCharacter = text[i];
		}
	}
	Device_Base::GetInstance()->PushMatrix();
	Device_Base::GetInstance()->Translate(xIn, yIn, 0.0f);
	Device_Base::GetInstance()->Scale(fontScaleGlobal, fontScaleGlobal, 1.0);
	Device_Base::GetInstance()->UnbindVBO(VBO::VBO_TARGET_ARRAY_BUFFER);
	Device_Base::GetInstance()->SetAttributePointer(ATTRIBUTE_FLOAT3_POSITION, buffer, sizeof(float) * 5);
	Device_Base::GetInstance()->SetAttributePointer(ATTRIBUTE_FLOAT2_TEXCOORD_DIFFUSE, buffer + 3, sizeof(float) * 5);
	Device_Base::GetInstance()->UpdateVertexAttribPointer();
	Device_Base::GetInstance()->SetUniformTexture(UNIFORM_TEXTURE_DIFFUSE, font->texture);
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
	Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, 0, validCharacter * 6);
	Device_Base::GetInstance()->PopMatrix();
	SafeDeleteArray(buffer);
}

void FontManager::DrawUTF8(const float &xIn, const float &yIn, Font* font, const char* textUTF8) //Heavy call, only for debug
{
	int length = 0;
	unsigned long* text = NULL;
	Convert_UTF8_To_UTF32(textUTF8, &text, &length);
	DrawUTF8(xIn, yIn, font, text, length);
	SafeDeleteArray(text);
}

void FontManager::DrawUTF8(const float &xIn, const float &yIn, Font* font, const unsigned long* text, int length) //Heavy call, only for debug
{
	int xPointer = 0;
	int yPointer = 0;
	float z = -1.0f;

	float* buffer = new float[length * 6 * 5];
	float* pointer = buffer;
	int validCharacter = 0;
	long long previousCharacter = 0;
	for (int i = 0; i < length; i++)
	{
		if (text[i] == '\n')
		{
			xPointer = 0;
			yPointer -= font->lineHeight;
			continue;
		}
		std::map<int, FontCharacter>::iterator it = font->characterMap.find((int)text[i]);
		if (it != font->characterMap.end())
		{
			if (previousCharacter != 0)
			{
				long long  second = text[i];
				long long key = (previousCharacter << 32) | second;
				std::map<long long, int>::iterator it2 = font->kerningMap.find(key);
				if (it2 != font->kerningMap.end())
				{
					xPointer += it2->second;
				}
			}
			int x = xPointer + it->second.xoffset;
			int y = yPointer + it->second.yoffset;
			pointer[0] = x;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			pointer[0] = x;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordLeft;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y - it->second.height;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordBottom;
			pointer += 5;
			//----------------------------------
			pointer[0] = x + it->second.width;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = it->second.texcoordRight;
			pointer[4] = it->second.texcoordTop;
			pointer += 5;
			//----------------------------------
			xPointer += it->second.xadvance;
			validCharacter++;
			previousCharacter = text[i];
		}
	}
	Device_Base::GetInstance()->PushMatrix();
	Device_Base::GetInstance()->Translate(xIn, yIn, 0.0f);
	Device_Base::GetInstance()->Scale(fontScaleGlobal, fontScaleGlobal, 1.0);
	Device_Base::GetInstance()->UnbindVBO(VBO::VBO_TARGET_ARRAY_BUFFER);
	Device_Base::GetInstance()->SetAttributePointer(ATTRIBUTE_FLOAT3_POSITION, buffer, sizeof(float) * 5);
	Device_Base::GetInstance()->SetAttributePointer(ATTRIBUTE_FLOAT2_TEXCOORD_DIFFUSE, buffer + 3, sizeof(float) * 5);
	Device_Base::GetInstance()->UpdateVertexAttribPointer();
	Device_Base::GetInstance()->SetUniformTexture(UNIFORM_TEXTURE_DIFFUSE, font->texture);
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
	Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, 0, validCharacter * 6);
	Device_Base::GetInstance()->PopMatrix();
	SafeDeleteArray(buffer);
}

void FontManager::DrawWrapASCII(const float &xIn, const float &yIn, Font* font, const char* text, float width)
{

}

void FontManager::WrapTextUTF8(const char* textUTF8, float width, Font* font, unsigned long** output, int* lengthOutput)
{
	Convert_UTF8_To_UTF32(textUTF8, output, lengthOutput);
	unsigned long* text = *output;
	int length = *lengthOutput;

	int xPointer = 0;
	long long previousCharacter = 0;
	int currentWidth = 0;
	float scaleWidth = width / FontManager::GetInstance()->fontScaleGlobal;
	for (int i = 0; i < length - 1; i++)
	{
		if (text[i] == '\n')
		{
			currentWidth = 0;
			continue;
		}
		

		std::map<int, FontCharacter>::iterator it = font->characterMap.find((int)text[i]);
		if (it != font->characterMap.end())
		{
			if (previousCharacter != 0)
			{
				long long  second = text[i];
				long long key = (previousCharacter << 32) | second;
				std::map<long long, int>::iterator it2 = font->kerningMap.find(key);
				if (it2 != font->kerningMap.end())
				{
					currentWidth += it2->second;
				}
			}

			currentWidth += it->second.xadvance;
			previousCharacter = text[i];
		}

		if (text[i] == ' ' && text[i + 1] != ' ') //New word
		{
			long long previousCharacter2 = text[i];
			int currentWidth2 = currentWidth;
			for (int j = i + 1; j < length; j++)
			{
				if (text[j] == '\n' || text[j] == ' ')
				{
					break;
				}
				std::map<int, FontCharacter>::iterator it = font->characterMap.find((int)text[j]);
				if (it != font->characterMap.end())
				{
					if (previousCharacter2 != 0)
					{
						long long  second = text[j];
						long long key = (previousCharacter2 << 32) | second;
						std::map<long long, int>::iterator it2 = font->kerningMap.find(key);
						if (it2 != font->kerningMap.end())
						{
							currentWidth2 += it2->second;
						}
					}

					currentWidth2 += it->second.xadvance;
					previousCharacter2 = text[j];
				}
				if (currentWidth2 > scaleWidth)
				{
					text[i] = '\n';
					previousCharacter = 0;
					currentWidth = 0;
					break;
				}
			}
		}
	}
}