#ifdef WIN32
#include <windows.h>
#endif
#include "Common.h"
void SafeCopyWCHAR(wchar_t** dest, const wchar_t* source)
{
	SafeDeleteArray(*dest);
	size_t length = wcslen(source);
	*dest = new wchar_t[length+1];
	wcscpy(*dest, source);
	(*dest)[length] = 0;
}
void SafeCopyCHAR(char** dest, const char* source)
{
	SafeDeleteArray(*dest);
	size_t length = strlen(source);
	*dest = new char[length+1];
	strcpy(*dest, source);
	(*dest)[length] = 0;
}
void SafeAppendCHAR(char** dest, const char* source)
{
	size_t length = strlen(*dest) + strlen(source);
	char* temp = new char[length+1];
	strcpy(temp, *dest);
	strcat(temp, source);
	temp[length] = 0;
	SafeDeleteArray(*dest);
	*dest = temp;
}
void SafeMergeCHAR(char** dest, const char* source1, const char* source2)
{
	SafeDeleteArray(*dest);
	size_t length = strlen(source1) + strlen(source2);
	*dest = new char[length+1];
	strcpy(*dest, source1);
	strcat(*dest, source2);
	(*dest)[length] = 0;
}
void ConvertCharToWchar(wchar_t* dest, const char* source)
{
	int len = strlen(source);
	mbstowcs(dest, source, len);
	dest[len] = 0;
}
void ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length)
{
	FILE *file = fopen(fileName, readMode);
	*length = 0;
	SafeDeleteArray(*buffer);
	if (file)
	{
		fseek(file, 0, SEEK_END);
		*length = ftell(file);
		fseek(file, 0, SEEK_SET);
		*buffer = new char[*length + 1];
		memset(*buffer, 0, *length + 1);
		fread(*buffer, 1, *length, file);
		fclose(file);
	}
}
void DebugLog(const char* log)
{
	MessageBoxA(NULL, log, "Error!", MB_OK | MB_ICONINFORMATION);
}
std::string GetFolderPath(const char* path)
{
	int index = max((int)strrchr(path, '\\'), (int)strrchr(path, '/'));
	if (index == 0)
	{
		return "";
	}
	std::string result;
	result.append(path, index - (int)path);
	return result;
}

void Convert_UTF8_To_UTF32(const char* src, unsigned long** dest, int* length)
{
	SafeDeleteArray(*dest);
	int lengthSrc = strlen(src);
	*dest = new unsigned long[lengthSrc + 1];
	memset(*dest, 0, sizeof(unsigned long) * lengthSrc + 1);
	int index = 0;
	for (int i = 0; i < lengthSrc; i++)
	{
		unsigned long byte1 = (unsigned char)src[i];
		if (byte1 < 0x80) // 1 byte
		{
			(*dest)[index++] = byte1;
		}
		else if (byte1 < 0xE0) // 2 byte
		{
			unsigned long byte2 = (unsigned char)src[i + 1];
			(*dest)[index++] = ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
			i += 1;
		}
		else if (byte1 < 0xF0) // 3 byte
		{
			unsigned long byte2 = (unsigned char)src[i + 1];
			unsigned long byte3 = (unsigned char)src[i + 2];
			(*dest)[index++] = ((byte1 & 0xF) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
			i += 2;
		}
		else if (byte1 < 0xF8) // 4 byte
		{
			unsigned long byte2 = (unsigned char)src[i + 1];
			unsigned long byte3 = (unsigned char)src[i + 2];
			unsigned long byte4 = (unsigned char)src[i + 3];
			(*dest)[index++] = ((byte1 & 0x7) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
			i += 3;
		}
	}
	*length = index;
}