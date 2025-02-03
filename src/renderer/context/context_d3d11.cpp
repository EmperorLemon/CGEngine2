#include <GLFW/glfw3.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdio>

#include "context.h"

// context_d3d11.cpp
namespace cg::renderer::ContextOps
{
	template <typename T>
	static constexpr T GetD3D11COM(void* ptr) { return reinterpret_cast<T>(ptr); }

	static bool CreateDeviceAndSwapchain(const CGSwapchainConfig& swapchainConfig, HWND nativeWindowHandle, CGD3D11Context& context, bool debug);
	static bool QueryPhysicalDevice(CGD3D11Context& context, CGPhysicalDeviceProperties& deviceProperties);
	static bool CreateDebugInterface(CGD3D11Context& context);
	static bool CreateRenderTargetView(CGD3D11Context& context);

	bool D3D11Init(CGRenderContext& renderContext)
	{
		renderContext.apiFunctions.init = glfwInit;
		renderContext.apiFunctions.shutdown = glfwTerminate;

		if (!renderContext.apiFunctions.init())
		{
			return false;
		}

		// Set GLFW window hints
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		return true;
	}

	bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext, const bool debug)
	{
		// 1. Create device and context
		if (!CreateDeviceAndSwapchain(swapchainConfig, static_cast<HWND>(nativeWindowHandle), renderContext.d3d11Context, debug))
		{
			return false;
		}

		QueryPhysicalDevice(renderContext.d3d11Context, renderContext.deviceProperties);

		// *Optional* Create debug interface
		if (debug)
		{
			if (!CreateDebugInterface(renderContext.d3d11Context))
			{
				return false;
			}
		}

		// 2. Create backbuffer and render target view
		if (!CreateRenderTargetView(renderContext.d3d11Context))
		{
			return false;
		}

		SetupD3D11ContextFunctions(renderContext);

		return true;
	}

	bool CreateDeviceAndSwapchain(const CGSwapchainConfig& swapchainConfig, HWND nativeWindowHandle, CGD3D11Context& context, const bool debug)
	{
		UINT createDeviceFlags = 0U;

		if (debug)
		{
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferCount = swapchainConfig.bufferCount;
		scd.BufferDesc.Width = swapchainConfig.width;
		scd.BufferDesc.Height = swapchainConfig.height;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate = DXGI_RATIONAL{ 0U, 1U };
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = nativeWindowHandle;
		scd.SampleDesc = { 1U, 0U };
		scd.Windowed = swapchainConfig.windowed;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,												   // Default adapter
			D3D_DRIVER_TYPE_HARDWARE,								   // Hardware driver
			nullptr,												   // No software driver
			createDeviceFlags,										   // Debug flags, etc.
			&featureLevel, 1U,										   // Default feature level
			D3D11_SDK_VERSION,
			&scd,													   // Swapchain description
			GetD3D11COM<IDXGISwapChain**>(&context.swapchain),		   // Output swapchain
			GetD3D11COM<ID3D11Device**>(&context.device),			   // Output device
			nullptr,												   // Actual feature level (optional)
			GetD3D11COM<ID3D11DeviceContext**>(&context.deviceContext) // Output device context
		);

		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	bool QueryPhysicalDevice(CGD3D11Context& context, CGPhysicalDeviceProperties& deviceProperties)
	{
		IDXGIDevice* dxgiDevice = nullptr;
		HRESULT result = GetD3D11COM<ID3D11Device*>(context.device)->QueryInterface(IID_PPV_ARGS(&dxgiDevice));

		if (FAILED(result))
		{
			return false;
		}

		IDXGIAdapter* adapter = nullptr;
		result = dxgiDevice->GetAdapter(&adapter);
		dxgiDevice->Release();

		if (FAILED(result))
		{
			return false;
		}

		DXGI_ADAPTER_DESC desc = {};
		result = adapter->GetDesc(&desc);
		adapter->Release();

		if (FAILED(result))
		{
			return false;
		}

		// deviceProperties.vendor

		return true;
	}

	bool CreateDebugInterface(CGD3D11Context& context)
	{
		// Get debug interface
		ID3D11Debug* debugDevice = nullptr;
		HRESULT result = GetD3D11COM<ID3D11Device*>(context.device)->QueryInterface(IID_PPV_ARGS(&debugDevice));

		if (FAILED(result))
		{
			return false;
		}

		debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		// Enable object tracking
		ID3D11InfoQueue* infoQueue = nullptr;
		result = debugDevice->QueryInterface(IID_PPV_ARGS(&infoQueue));

		if (FAILED(result))
		{
			debugDevice->Release();
			return false;
		}

		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

		infoQueue->Release();
		debugDevice->Release();

		return true;
	}

	bool CreateRenderTargetView(CGD3D11Context& context)
	{
		// Create backbuffer
		ID3D11Texture2D* backbuffer = nullptr;
		HRESULT result = GetD3D11COM<IDXGISwapChain*>(context.swapchain)->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

		if (FAILED(result))
		{
			return false;
		}

		// Create render target view
		result = GetD3D11COM<ID3D11Device*>(context.device)->CreateRenderTargetView(
			backbuffer, 
			nullptr, 
			GetD3D11COM<ID3D11RenderTargetView**>(&context.renderTargets[0])
		);

		if (FAILED(result))
		{
			return false;
		}

		backbuffer->Release();
		context.renderTargetCount = 1;

		const auto deviceContext = GetD3D11COM<ID3D11DeviceContext*>(context.deviceContext);

		deviceContext->OMSetRenderTargets(1, GetD3D11COM<ID3D11RenderTargetView* const*>(&context.renderTargets[0]), nullptr);

		return true;
	}

	static void Present(void* swapchain_)
	{
		if (!swapchain_)
		{
			return;
		}

		const auto swapchain = static_cast<IDXGISwapChain*>(swapchain_);
		swapchain->Present(1, 0);
	}

	void SetupD3D11ContextFunctions(CGRenderContext& renderContext)
	{
		renderContext.apiFunctions.present = Present;
	}

	void DestroyD3D11Context(CGRenderContext& renderContext)
	{
		for (uint8_t i = 0; i < renderContext.d3d11Context.renderTargetCount; ++i)
		{
			if (renderContext.d3d11Context.renderTargets[i])
			{
				GetD3D11COM<ID3D11RenderTargetView*>(renderContext.d3d11Context.renderTargets[i])->Release();
				renderContext.d3d11Context.renderTargets[i] = nullptr;
			}
		}

		renderContext.d3d11Context.renderTargetCount = 0;

		if (renderContext.d3d11Context.deviceContext)
		{
			GetD3D11COM<ID3D11DeviceContext*>(renderContext.d3d11Context.deviceContext)->Release();
			renderContext.d3d11Context.deviceContext = nullptr;
		}

		if (renderContext.d3d11Context.swapchain)
		{
			GetD3D11COM<IDXGISwapChain*>(renderContext.d3d11Context.swapchain)->Release();
			renderContext.d3d11Context.swapchain = nullptr;
		}

		if (renderContext.d3d11Context.device)
		{
			GetD3D11COM<ID3D11Device*>(renderContext.d3d11Context.device)->Release();
			renderContext.d3d11Context.device = nullptr;
		}
	}
}
