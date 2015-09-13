#include "StringManager.h"
#include "tinyxml2\tinyxml2.h"

void StringManager::Init(const char* filePath)
{
	SafeDelete(instance);
	instance = new StringManager();
	char* buffer = NULL;
	int length = 0;
	ReadFileToMemory(filePath, "rt", &buffer, &length);
	if (length <= 0)
	{
		return;
	}
	tinyxml2::XMLDocument doc;
	doc.Parse(buffer);
	SafeDeleteArray(buffer);
	if (doc.ErrorID() != 0)
	{
		return;
	}
	tinyxml2::XMLElement* root = doc.FirstChildElement("data-set");
	tinyxml2::XMLElement* recordXML = root->FirstChildElement("record");
	while (recordXML)
	{
		StringRecord* record = new StringRecord();
		const char* ID = recordXML->FirstChildElement("ID")->GetText();
		const char* EN = recordXML->FirstChildElement("EN")->GetText();
		const char* VN = recordXML->FirstChildElement("VN")->GetText();
		SafeCopyCHAR(&record->ID, ID);
		SafeCopyCHAR(&record->EN, EN);
		SafeCopyCHAR(&record->VN, VN);
		instance->stringMap[ID] = record;
		recordXML = recordXML->NextSiblingElement("record");
	}
}