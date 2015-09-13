#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H
#include "Texture.h"
#include "tinyxml2\tinyxml2.h"
#include <map>


struct FontCharacter
{
	float texcoordLeft, texcoordTop, texcoordRight, texcoordBottom;
	int width, height, xoffset, yoffset, xadvance;
};
class Font
{
public:
	Texture* texture;
	int lineHeight;
	int textureOrginalWidth;
	int textureOrginalHeight;
	std::map<int, FontCharacter> characterMap;
	std::map<long long, int> kerningMap;
	char* fileName;
	Font()
	{
		fileName = NULL;
	}
	~Font()
	{
		SafeDeleteArray(fileName);
	}
};
class FontManager : public Singleton<FontManager>
{
	std::vector<Font*> fontList;
	char* directory;
public:
	FontManager()
	{
		directory = NULL;
	}
	~FontManager()
	{
		for (int i = 0; i<fontList.size(); i++)
		{
			SafeDelete(fontList[i]);
		}
		fontList.clear();
		SafeDeleteArray(directory);
	}
	static float fontScaleGlobal;
	static void Init(const char* directory = NULL);
	static void Release()
	{
		SafeDelete(instance);
	}
	Font* LoadFontFromFile(const char* filename);
	Font* LoadFontFromXML(const char* buffer);
	Font* GetFontByFileName(const char* filename);
	void SetDirectory(char* directory)
	{
		SafeCopyCHAR(&this->directory, directory);
	}
	const char* GetDirectory()
	{
		return directory;
	}
	void DrawASCII(const float &xIn, const float &yIn, Font* font, const char* text); //Heavy call, only for debug
	void DrawWrapASCII(const float &xIn, const float &yIn, Font* font, const char* text, float width); //Heavy call, only for debug
	void DrawUTF8(const float &xIn, const float &yIn, Font* font, const char* text); //Heavy call, only for debug
	void DrawUTF8(const float &xIn, const float &yIn, Font* font, const unsigned long* text, int length); //Heavy call, only for debug
	void WrapTextUTF8(const char* textUTF8, float width, Font* font, unsigned long** output, int* lengthOutput);
};
#endif