#include "DXDeviceWP8.h"
// Initialize the Direct3D resources required to run.
using namespace DirectX;
DXDeviceWP8::DXDeviceWP8(void* params[])
{
	UpdateDevice((ID3D11Device1*)params[0], (ID3D11DeviceContext1*)params[1], (ID3D11RenderTargetView*)params[2]);
}
void DXDeviceWP8::InitSpecificDevice()
{
	
}
// Recreate all device resources and set them back to the current state.
/*
void DXDeviceWP8::HandleDeviceLost()
{
	// Reset these member variables to ensure that UpdateForWindowSizeChange recreates all resources.
	m_windowBounds.Width = 0;
	m_windowBounds.Height = 0;
	m_swapChain = nullptr;

	CreateDeviceResources();
	UpdateForWindowSizeChange();
}
*/
// These are the resources that depend on the device.
void DXDeviceWP8::CreateDeviceResources()
{
	
}
void DXDeviceWP8::UpdateDevice(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView)
{
	m_d3dContext = context;
	if(renderTargetMain == NULL)
	{
		renderTargetMain = new RenderTarget("main");
	}
	renderTargetMain->renderTargetView = renderTargetView;

	if (m_d3dDevice == NULL)
	{
		m_d3dDevice = device;
		CreateDeviceResources();
	}
	else if (m_d3dDevice != device)
	{
		m_d3dDevice = device;
		CreateDeviceResources();

		// Force call to CreateWindowSizeDependentResources.
		m_renderTargetSize.Width  = -1;
		m_renderTargetSize.Height = -1;
	}
	
	ID3D11Resource* renderTargetViewResource;
	renderTargetMain->renderTargetView->GetResource(&renderTargetViewResource);
	
	ID3D11Texture2D* backBuffer;
	backBuffer = (ID3D11Texture2D*)renderTargetViewResource;

	// Cache the rendertarget dimensions in our helper class for convenient use.
    D3D11_TEXTURE2D_DESC backBufferDesc;
    backBuffer->GetDesc(&backBufferDesc);
	
    if (m_renderTargetSize.Width  != static_cast<float>(backBufferDesc.Width) ||
        m_renderTargetSize.Height != static_cast<float>(backBufferDesc.Height))
    {
        m_renderTargetSize.Width  = static_cast<float>(backBufferDesc.Width);
        m_renderTargetSize.Height = static_cast<float>(backBufferDesc.Height);
		this->m_Width = m_renderTargetSize.Width;
		this->m_Height = m_renderTargetSize.Height;
        CreateWindowSizeDependentResources();
    }
	
	// Set the rendering viewport to target the entire window.
	SetViewport(0.0f,
		0.0f,
		m_renderTargetSize.Width,
		m_renderTargetSize.Height);
	m_d3dContext->OMSetRenderTargets(
		1,
		&renderTargetMain->renderTargetView,
		renderTargetMain->depthStencilView
		);
	m_d3dContext->OMSetDepthStencilState(pDepthStencilStateCurrent, 1);
}
// Allocate all memory resources that depend on the window size.
void DXDeviceWP8::CreateWindowSizeDependentResources()
{
	// Create a depth stencil view.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(m_renderTargetSize.Width),
		static_cast<UINT>(m_renderTargetSize.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
		);

	ID3D11Texture2D* depthStencil;
	m_d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencil
			);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	m_d3dDevice->CreateDepthStencilView(
			depthStencil,
			&depthStencilViewDesc,
			&renderTargetMain->depthStencilView
			);
}

// This method is called in the event handler for the SizeChanged event.
void DXDeviceWP8::UpdateForWindowSizeChange(float width, float height)
{
	m_windowBounds.Width  = width;
	m_windowBounds.Height = height;
}

void DXDeviceWP8::ReleaseResourcesForSuspending()
{
	// Phone applications operate in a memory-constrained environment, so when entering
	// the background it is a good idea to free memory-intensive objects that will be
	// easy to restore upon reactivation. The swapchain and backbuffer are good candidates
	// here, as they consume a large amount of memory and can be reinitialized quickly.
	m_swapChain = nullptr;
	renderTargetMain->renderTargetView = nullptr;
	renderTargetMain->depthStencilView = nullptr;
}

// Method to deliver the final image to the display.
/*
void DXDeviceWP8::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	m_d3dContext->DiscardView(m_renderTargetView);

	// Discard the contents of the depth stencil.
	m_d3dContext->DiscardView(m_depthStencilView);

	// If the device was removed either by a disconnect or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		HandleDeviceLost();
	}
	else
	{
		//DX::ThrowIfFailed(hr);
	}
}
*/
// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float DXDeviceWP8::ConvertDipsToPixels(float dips)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * Windows::Graphics::Display::DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}
void DXDeviceWP8::Resume()
{
	CreateWindowSizeDependentResources();
}
void DXDeviceWP8::Suspend()
{
	ReleaseResourcesForSuspending();
}
HRESULT DXDeviceWP8::CreateVertexBufferFromVerticesData(float* verticesData, UINT dataLength, ID3D11Buffer** vBufferOuput)
{
	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth        = sizeof( float ) * dataLength;
	bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags   = 0;
	bufferDesc.MiscFlags        = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = verticesData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	return m_d3dDevice->CreateBuffer( &bufferDesc, &InitData, vBufferOuput );
}
/*
void DXDeviceWP8::FlipBuffer()
{
	Present();
}
*/
void DXDeviceWP8::ClearColorBuffer(float rgba[4])
{
	m_d3dContext->ClearRenderTargetView(
		GetCurrentRenderTarget()->renderTargetView,
		rgba
		);

}
void DXDeviceWP8::ClearDepthStencilBuffer()
{
	m_d3dContext->ClearDepthStencilView(
		GetCurrentRenderTarget()->depthStencilView,
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
		);

}
