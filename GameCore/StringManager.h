#ifndef STRING_MANAGER_H
#define STRING_MANAGER_H
#include "Common.h"
#include <map>

struct StringRecord
{
	char* ID;
	char* EN;
	char* VN;
	StringRecord()
	{
		ID = NULL;
		EN = NULL;
		VN = NULL;
	}
	~StringRecord()
	{
		SafeDeleteArray(ID);
		SafeDeleteArray(EN);
		SafeDeleteArray(VN);
	}
};
class StringManager : public Singleton<StringManager>
{
	std::map<std::string, StringRecord*> stringMap;
public:
	~StringManager()
	{
		for (std::map<std::string, StringRecord*>::iterator it = stringMap.begin(); it != stringMap.end(); it++)
		{
			SafeDelete(it->second);
		}
		stringMap.clear();
	}
	static void Init(const char* filePath);
	static void Release()
	{
		SafeDelete(instance);
	}
	StringRecord* GetStringRecord(const char* key)
	{
		std::map<std::string, StringRecord*>::iterator it = stringMap.find(key);
		if (it != stringMap.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}
};
#endif