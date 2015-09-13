#ifndef XCOMMON_H
#define XCOMMON_H
#include <string>
template< typename T > void SafeDelete( T*& pVal )
{
    delete pVal;
    pVal = 0;
}

template< typename T > void SafeDeleteArray( T*& pVal )
{
	if (pVal)
	{
		delete[] pVal;
	}
    pVal = 0;
}
template< typename T > void SafeRelease( T*& pVal )
{
    if(pVal)
	{
		pVal->Release();
		pVal = 0;
	}
}
template< typename T1, typename T2 > class CreateObjectClass
{
public:
    static T1* CreateObject()
	{
		return (T1*)(new T2());
	}
};
template <class T>
class Singleton
{
public:
	//~Singleton(void) {};
    static T* GetInstance(void) {return instance;}
	static void Release()
	{
		SafeDelete(instance);
	}
	static void SetInstance(T* instanceIn)
	{
		instance = instanceIn;
	}
protected:
    //Singleton<T>(void) {};
    static T* instance;
};
template < class T >
T* Singleton< T >::instance = 0;
void SafeCopyWCHAR(wchar_t** dest, const wchar_t* source);
void SafeCopyCHAR(char** dest, const char* source);
void SafeAppendCHAR(char** dest, const char* source);
void SafeMergeCHAR(char** dest, const char* source1, const char* source2);
void ConvertCharToWchar(wchar_t* dest, const char* source);
void ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length);
void DebugLog(const char* log);
std::string GetFolderPath(const char* path); //without \/
void Convert_UTF8_To_UTF32(const char* src, unsigned long** dest, int* length);
#endif