#include <GLFW/glfw3.h>

#include "core/logger.hpp"
#include "context_d3d11.h"

// context_d3d11.cpp
namespace cg::renderer
{
	void D3D11Init(CGRenderContext& renderContext)
	{
		renderContext.SetData(std::make_unique<CGRenderContextData<CGRendererType::Direct3D11>>());

		renderContext.Init = []()
			{
				return glfwInit();
			};

		renderContext.Shutdown = []()
			{
				glfwTerminate();
			};
	}

	static bool CreateRenderTargetView(CGRenderContextData<CGRendererType::Direct3D11>* data);

	bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext)
	{
		auto data = renderContext.GetData<CGRendererType::Direct3D11>();

		if (!data)
			return false;

		// 1. Create device and context
		UINT createDeviceFlags = 0U;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else
		if (renderContext.GetDebug())
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = swapchainConfig.bufferCount;
		desc.BufferDesc.Width = swapchainConfig.width;
		desc.BufferDesc.Height = swapchainConfig.height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate = { 60, 1 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = static_cast<HWND>(nativeWindowHandle);
		desc.SampleDesc = { 1, 0 };
		desc.Windowed = swapchainConfig.windowed;

		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,						   // Default adapter
			D3D_DRIVER_TYPE_HARDWARE,		   // Hardware driver
			nullptr,						   // No software driver
			createDeviceFlags,				   // Debug flags, etc.
			nullptr, 0U,					   // Default feature level
			D3D11_SDK_VERSION,
			&desc,						       // Swapchain description
			data->swapchain.GetAddressOf(),    // Output swapchain
			data->device.GetAddressOf(),	   // Output device
			nullptr,						   // Actual feature level (optional)
			data->deviceContext.GetAddressOf() // Output device context
		);

		if (FAILED(result))
			return false;

		// 2. Create backbuffer and render target view
		if (!CreateRenderTargetView(data))
			return false;

		SetupD3D11ContextFunctions(nativeWindowHandle, renderContext);

		return true;
	}

	bool CreateRenderTargetView(CGRenderContextData<CGRendererType::Direct3D11>* data)
	{
		// Create backbuffer
		ComPtr<ID3D11Texture2D> backbuffer;
		HRESULT result = data->swapchain->GetBuffer(0, IID_PPV_ARGS(backbuffer.GetAddressOf()));
		
		if (FAILED(result))
			return false;

		// Create render target view
		result = data->device->CreateRenderTargetView(backbuffer.Get(), nullptr, data->renderTargetView.GetAddressOf());

		if (FAILED(result))
			return false;

		data->deviceContext->OMSetRenderTargets(1, data->renderTargetView.GetAddressOf(), nullptr);

		return true;
	}

	void SetupD3D11ContextFunctions(void* nativeWindowHandle, CGRenderContext& renderContext)
	{
		auto data = renderContext.GetData<CGRendererType::Direct3D11>();

		renderContext.Present = [swapchain = data->swapchain]()
			{
				if (swapchain)
					swapchain->Present(1, 0);
			};
	}
}