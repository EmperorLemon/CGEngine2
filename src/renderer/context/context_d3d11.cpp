#include <GLFW/glfw3.h>

#include "context_d3d11.h"

using Microsoft::WRL::ComPtr;

// context_d3d11.cpp
namespace cg::renderer::ContextOps
{
	//// Forward declarations
	//static bool CreateDeviceAndSwapchain(const CGSwapchainConfig& swapchainConfig, HWND nativeWindowHandle, CGRenderContextData<CGRendererType::Direct3D11>* data, bool debug);
	//static bool CreateRenderTargetView(CGRenderContextData<CGRendererType::Direct3D11>* data);

	//bool D3D11Init(CGRenderContext& renderContext)
	//{
	//	auto& renderContextAPIFunctions = renderContext.GetRenderContextAPIFunctions();

	//	renderContext.SetData(std::make_unique<CGRenderContextData<CGRendererType::Direct3D11>>());

	//	renderContextAPIFunctions.init = []()
	//	{
	//		return glfwInit();
	//	};

	//	renderContextAPIFunctions.shutdown = []()
	//	{
	//		glfwTerminate();
	//	};

	//	if (!renderContext.Init())
	//	{
	//		return false;
	//	}

	//	// Set GLFW window hints
	//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//	return true;
	//}

	//bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext, const bool debug)
	//{
	//	auto data = renderContext.GetData<CGRendererType::Direct3D11>();

	//	if (!data)
	//	{
	//		return false;
	//	}

	//	// 1. Create device and context
	//	if (!CreateDeviceAndSwapchain(swapchainConfig, static_cast<HWND>(nativeWindowHandle), data, debug))
	//	{
	//		return false;
	//	}

	//	// 2. Create backbuffer and render target view
	//	if (!CreateRenderTargetView(data))
	//	{
	//		return false;
	//	}

	//	SetupD3D11ContextFunctions(nativeWindowHandle, renderContext);

	//	return true;
	//}

	//bool CreateDeviceAndSwapchain(const CGSwapchainConfig& swapchainConfig, HWND nativeWindowHandle, CGRenderContextData<CGRendererType::Direct3D11>* data, const bool debug)
	//{
	//	UINT createDeviceFlags = 0U;

	//	if (debug)
	//	{
	//		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//	}

	//	DXGI_SWAP_CHAIN_DESC scd = {};
	//	scd.BufferCount = swapchainConfig.bufferCount;
	//	scd.BufferDesc.Width = swapchainConfig.width;
	//	scd.BufferDesc.Height = swapchainConfig.height;
	//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	scd.BufferDesc.RefreshRate = DXGI_RATIONAL{ 0, 1 };
	//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//	scd.OutputWindow = nativeWindowHandle;
	//	scd.SampleDesc = { 1, 0 };
	//	scd.Windowed = swapchainConfig.windowed;
	//	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//	HRESULT result = D3D11CreateDeviceAndSwapChain(
	//		nullptr,						   // Default adapter
	//		D3D_DRIVER_TYPE_HARDWARE,		   // Hardware driver
	//		nullptr,						   // No software driver
	//		createDeviceFlags,				   // Debug flags, etc.
	//		nullptr, 0U,					   // Default feature level
	//		D3D11_SDK_VERSION,
	//		&scd,						       // Swapchain description
	//		data->swapchain.GetAddressOf(),    // Output swapchain
	//		data->device.GetAddressOf(),	   // Output device
	//		nullptr,						   // Actual feature level (optional)
	//		data->deviceContext.GetAddressOf() // Output device context
	//	);

	//	if (FAILED(result))
	//	{
	//		return false;
	//	}

	//	return true;
	//}

	//bool CreateRenderTargetView(CGRenderContextData<CGRendererType::Direct3D11>* data)
	//{
	//	// Create backbuffer
	//	ComPtr<ID3D11Texture2D> backbuffer;
	//	HRESULT result = data->swapchain->GetBuffer(0, IID_PPV_ARGS(backbuffer.GetAddressOf()));

	//	if (FAILED(result))
	//	{
	//		return false;
	//	}

	//	// Create render target view
	//	result = data->device->CreateRenderTargetView(backbuffer.Get(), nullptr, data->renderTargetView.GetAddressOf());

	//	if (FAILED(result))
	//	{
	//		return false;
	//	}

	//	data->deviceContext->OMSetRenderTargets(1, data->renderTargetView.GetAddressOf(), nullptr);

	//	return true;
	//}

	//void SetupD3D11ContextFunctions([[maybe_unused]] void* nativeWindowHandle, CGRenderContext& renderContext)
	//{
	//	auto data = renderContext.GetData<CGRendererType::Direct3D11>();

	//	auto& renderContextAPIFunctions = renderContext.GetRenderContextAPIFunctions();

	//	renderContextAPIFunctions.present = [swapchain = data->swapchain]()
	//	{
	//		if (swapchain)
	//		{
	//			swapchain->Present(1, 0);
	//		}
	//	};
	//}
}
