#include "LoadingManager.h"
#include "Device_Base.h"
#ifdef USE_DIRECTX
#include "DXDeviceWP8.h"
#endif

void LoadingManager::AddLoadingVertexShaderTask(VertexShader* vs)
{
#if WP8_IO_ASYNC
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif
	LoadingTask* loadingTask = new LoadingTask();
	loadingTask->type = LOADING_VSHADER;
	instance->loadingTaskList.push_back(loadingTask);
#ifdef WIN32
	FILE *file = fopen(vs->GetFileName(), "rt");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		long size = ftell(file);
		fseek (file, 0, SEEK_SET);
		char* buffer = new char[size];
		fread(buffer, 1, size, file);
		fclose(file);
		
		Device_Base::GetInstance()->CreateVertexShaderFromMemory(buffer, size, vs);
		SafeDeleteArray(buffer);

		loadingTask->isDone = true;
	}
#else
#if WP8_IO_ASYNC
	wchar_t* wfileName = new wchar_t[strlen(vs->GetFileName())+1];
	ConvertCharToWchar(wfileName, vs->GetFileName());
	//wcscat(wfileName, L".cso");
	Platform::String^ fileNameWithExtension =  ref new Platform::String(wfileName);
	SafeDeleteArray(wfileName);
	IOX::ReadDataAsync(fileNameWithExtension).then([loadingTask, vs] (Platform::Array<byte>^ fileData) 
	{
		
		DXDevice::GetInstance()->GetD3D11Device()->CreateVertexShader(
				fileData->Data,
				fileData->Length,
				nullptr,
				&(vs->dxVS));
		vs->Init(fileData->Data,
				fileData->Length);
		loadingTask->isDone = true;
	}
	);
#else
	FILE *file = fopen(vs->GetFileName(), "rb");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		long size = ftell(file);
		fseek (file, 0, SEEK_SET);
		char* buffer = new char[size];
		fread(buffer, 1, size, file);
		fclose(file);
		std::string descFileName = "Assets\\ShaderDesc\\";
		descFileName.append(vs->GetFileName(), strlen(vs->GetFileName()) - 3);
		descFileName.append("desc");
		file = fopen(descFileName.c_str(), "rt");
		char* bufferDesc = NULL;
		long sizeDesc = 0;
		if(file)
		{
			fseek (file, 0, SEEK_END);
			sizeDesc = ftell(file);
			fseek (file, 0, SEEK_SET);
			bufferDesc = new char[sizeDesc+1];
			bufferDesc[sizeDesc] = 0;
			fread(bufferDesc, 1, sizeDesc, file);
			fclose(file);
		}
		HRESULT result = DXDevice::GetInstance()->GetD3D11Device()->CreateVertexShader(
				buffer,
				size,
				nullptr,
				&(vs->dxVS));
		vs->Init(buffer,
				size, bufferDesc, sizeDesc);
		SafeDeleteArray(buffer);
		SafeDeleteArray(bufferDesc);
		loadingTask->isDone = true;
	}
#endif
#endif
}
void LoadingManager::AddLoadingPixelShaderTask(PixelShader* ps)
{
#if WP8_IO_ASYNC
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif
	LoadingTask* loadingTask = new LoadingTask();
	loadingTask->type = LOADING_VSHADER;
	instance->loadingTaskList.push_back(loadingTask);
#ifdef WIN32
	FILE *file = fopen(ps->GetFileName(), "rt");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		long size = ftell(file);
		fseek (file, 0, SEEK_SET);
		char* buffer = new char[size];
		fread(buffer, 1, size, file);
		fclose(file);
		
		Device_Base::GetInstance()->CreatePixelShaderFromMemory(buffer, size, ps);
		SafeDeleteArray(buffer);

		loadingTask->isDone = true;
	}
#else
#if WP8_IO_ASYNC
	wchar_t* wfileName = new wchar_t[strlen(ps->GetFileName())+1];
	ConvertCharToWchar(wfileName, ps->GetFileName());
	//wcscat(wfileName, L".cso");
	Platform::String^ fileNameWithExtension =  ref new Platform::String(wfileName);
	SafeDeleteArray(wfileName);
	IOX::ReadDataAsync(fileNameWithExtension).then([loadingTask, ps] (Platform::Array<byte>^ fileData) 
	{
		DXDevice::GetInstance()->GetD3D11Device()->CreatePixelShader(
				fileData->Data,
				fileData->Length,
				nullptr,
				&(ps->dxPS));
		ps->Init(fileData->Data,
				fileData->Length);
		loadingTask->isDone = true;
	}
	);
#else
	FILE *file = fopen(ps->GetFileName(), "rb");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		long size = ftell(file);
		fseek (file, 0, SEEK_SET);
		char* buffer = new char[size];
		fread(buffer, 1, size, file);
		fclose(file);
		std::string descFileName = "Assets\\ShaderDesc\\";
		descFileName.append(ps->GetFileName(), strlen(ps->GetFileName()) - 3);
		descFileName.append("desc");
		file = fopen(descFileName.c_str(), "rt");
		char* bufferDesc = NULL;
		long sizeDesc = 0;
		if(file)
		{
			fseek (file, 0, SEEK_END);
			sizeDesc = ftell(file);
			fseek (file, 0, SEEK_SET);
			bufferDesc = new char[sizeDesc+1];
			bufferDesc[sizeDesc] = 0;
			fread(bufferDesc, 1, sizeDesc, file);
			fclose(file);
		}
		HRESULT result = DXDevice::GetInstance()->GetD3D11Device()->CreatePixelShader(
				buffer,
				size,
				nullptr,
				&(ps->dxPS));
		ps->Init(buffer,
				size, bufferDesc, sizeDesc);
		SafeDeleteArray(buffer);
		SafeDeleteArray(bufferDesc);
		loadingTask->isDone = true;
	}
#endif
#endif
}

void LoadingManager::AddLoadingSpriteXMLTask(Sprite* sprite)
{
#if WP8_IO_ASYNC
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif
	LoadingTask* loadingTask = new LoadingTask();
	loadingTask->type = LOADING_SPRITE;
	instance->loadingTaskList.push_back(loadingTask);
	char* path = NULL;
	SafeMergeCHAR(&path, SpriteManager::GetInstance()->GetDirectory(), sprite->GetFileName());
#if WP8_IO_ASYNC
	wchar_t* wfileName = new wchar_t[strlen(path)+1];
	ConvertCharToWchar(wfileName, path);
	//wcscat(wfileName, L".sprite");
	Platform::String^ fileNameWithExtension =  ref new Platform::String(wfileName);
	SafeDeleteArray(wfileName);
	IOX::ReadDataAsync(fileNameWithExtension).then([loadingTask, sprite] (Platform::Array<byte>^ fileData) 
	{
		sprite->InitFromXML((char*)fileData->Data);
		loadingTask->isDone = true;
	}
	);
#else
	char* buffer = NULL;
	int length = 0;
	ReadFileToMemory(path, "rt", &buffer, &length);
	if (length > 0)
	{
		sprite->InitFromXML(buffer);
		SafeDeleteArray(buffer);
		loadingTask->isDone = true;
	}
	else
	{
		loadingTask->isDone = true;
	}
#endif
	SafeDeleteArray(path);
}
void LoadingManager::AddLoadingTextFile(const char* fileName, std::string* result)
{
#if WP8_IO_ASYNC
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif
	LoadingTask* loadingTask = new LoadingTask();
	loadingTask->type = LOADING_TEXT;
	instance->loadingTaskList.push_back(loadingTask);
#if WP8_IO_ASYNC
	wchar_t* wfileName = new wchar_t[strlen(fileName)+1];
	ConvertCharToWchar(wfileName, fileName);
	Platform::String^ fileNameWithExtension =  ref new Platform::String(wfileName);
	SafeDeleteArray(wfileName);
	IOX::ReadDataAsync(fileNameWithExtension).then([loadingTask, result] (Platform::Array<byte>^ fileData) 
	{
		*result = (char*)fileData->Data;
		loadingTask->isDone = true;
	}
	);
#else
	FILE *file = fopen(fileName, "rt");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		long size = ftell(file);
		fseek (file, 0, SEEK_SET);
		char* buffer = new char[size];
		fread(buffer, 1, size, file);
		fclose(file);
		*result = buffer;
		loadingTask->isDone = true;
	}
	else
	{
		loadingTask->isDone = true;
	}
#endif
}

void LoadingManager::AddLoadingBinaryFile(const char* fileName, char** result, unsigned int* size)
{
#if WP8_IO_ASYNC
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif
	LoadingTask* loadingTask = new LoadingTask();
	loadingTask->type = LOADING_TEXT;
	instance->loadingTaskList.push_back(loadingTask);
#if WP8_IO_ASYNC
	/* //Not implement
	wchar_t* wfileName = new wchar_t[strlen(fileName)+1];
	ConvertCharToWchar(wfileName, fileName);
	Platform::String^ fileNameWithExtension =  ref new Platform::String(wfileName);
	SafeDeleteArray(wfileName);
	IOX::ReadDataAsync(fileNameWithExtension).then([loadingTask, result] (Platform::Array<byte>^ fileData) 
	{
		*result = (char*)fileData->Data;
		loadingTask->isDone = true;
	}
	);
	*/
#else
	FILE *file = fopen(fileName, "rb");
	if(file)
	{
		fseek (file, 0, SEEK_END);
		*size = ftell(file);
		fseek (file, 0, SEEK_SET);
		*result = new char[*size];
		fread(*result, 1, *size, file);
		fclose(file);
		loadingTask->isDone = true;
	}
	else
	{
		loadingTask->isDone = true;
	}
#endif
}
void LoadingManager::Update()
{
	for(int i = GetTaskCount() - 1; i >= 0; i--)
	{
		if(loadingTaskList[i]->isDone)
		{
			loadingTaskList.erase(loadingTaskList.begin() + i);
		}
	}
}