#ifndef DXDEVICEWP8_H
#define DXDEVICEWP8_H
#include "DXDevice.h"
#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
#include <agile.h>
#define WP8_IO_ASYNC 0
namespace IOX
{
	inline Concurrency::task<Platform::Array<byte>^> ReadDataAsync(Platform::String^ filename)
	{
		using namespace Windows::Storage;
		using namespace Concurrency;
		
		auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
		
		return create_task(folder->GetFileAsync(filename)).then([] (StorageFile^ file) 
		{
			return file->OpenReadAsync();
		}).then([] (Streams::IRandomAccessStreamWithContentType^ stream)
		{
			unsigned int bufferSize = static_cast<unsigned int>(stream->Size);
			auto fileBuffer = ref new Streams::Buffer(bufferSize);
			return stream->ReadAsync(fileBuffer, bufferSize, Streams::InputStreamOptions::None);
		}).then([] (Streams::IBuffer^ fileBuffer) -> Platform::Array<byte>^ 
		{
			auto fileData = ref new Platform::Array<byte>(fileBuffer->Length);
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(fileData);
			return fileData;
		});
	}
};
class DXDeviceWP8: public DXDevice
{
	IDXGISwapChain1* m_swapChain;
	

	// Cached renderer properties.
	D3D_FEATURE_LEVEL m_featureLevel;
	Windows::Foundation::Size m_renderTargetSize;
	Windows::Foundation::Rect m_windowBounds;
	Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
	
public:
	DXDeviceWP8(void* params[]);
	virtual void InitSpecificDevice();
	//void HandleDeviceLost();
	virtual DeviceType GetDeviceType()
	{
		return DeviceType::DEVICE_WP8_XAML;
	}
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void UpdateForWindowSizeChange(float width, float height);
	void ReleaseResourcesForSuspending();
	//void Render();
	//void Present();
	void UpdateDevice(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView);
	float ConvertDipsToPixels(float dips);
	virtual void Resume();
	virtual void Suspend();
	virtual HRESULT CreateVertexBufferFromVerticesData(float* verticesData, UINT dataLength, ID3D11Buffer** vBufferOuput);
	//virtual void FlipBuffer();
	virtual void ClearColorBuffer(float rgba[4]);
	virtual void ClearDepthStencilBuffer();
protected:
	
};
#endif