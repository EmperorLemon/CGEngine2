#include <d3d11.h>
#include <dxgi.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "renderer.h"
#include "platform/window.h"

// renderer_d3d11.cpp
namespace cg::renderer
{
	namespace D3D11
	{
		template <typename T>
		static constexpr T GetD3D11COM(void* ptr)
		{
			return reinterpret_cast<T>(ptr);
		}

		//static void CALLBACK DebugCallback(D3D11_MESSAGE_CATEGORY Category, D3D11_MESSAGE_SEVERITY Severity, D3D11_MESSAGE_ID ID, LPCSTR Description, void* Context)
		//{
		//	switch (Severity)
		//	{
		//		case D3D11_MESSAGE_SEVERITY_CORRUPTION:
		//		{
		//			printf("CORRUPTION: %s\n", Description);
		//			break;
		//		}
		//		case D3D11_MESSAGE_SEVERITY_ERROR:
		//		{
		//			printf("ERROR: %s\n", Description);
		//			break;
		//		}
		//		case D3D11_MESSAGE_SEVERITY_WARNING:
		//		{
		//			printf("WARNING: %s\n", Description);
		//			break;
		//		}
		//		case D3D11_MESSAGE_SEVERITY_INFO:
		//		{
		//			printf("INFO: %s\n", Description);
		//			break;
		//		}
		//	}
		//}

		bool Init(CGRenderFunctions& functions)
		{
			functions.init = glfwInit;
			functions.shutdown = glfwTerminate;

			if (!functions.init())
			{
				return false;
			}

			// Set GLFW window hints
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			return true;
		}

		bool CreateDevice(CGRenderDevice& device, CGRenderContext& context, const bool debug)
		{
			if (device.api.d3d11.device || context.api.d3d11.context)
			{
				return false;
			}

			UINT createDeviceFlags = 0U;

			if (debug)
			{
				createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
			}

			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
			D3D_FEATURE_LEVEL actualFeatureLevel;

			HRESULT result = D3D11CreateDevice(
				nullptr,													   // Default adapter
				D3D_DRIVER_TYPE_HARDWARE,									   // Hardware driver
				nullptr,													   // Software driver
				createDeviceFlags,											   // Debug flags, etc.
				&featureLevel, 1U,											   // Default feature level
				D3D11_SDK_VERSION,
				GetD3D11COM<ID3D11Device**>(&device.api.d3d11.device),		   // Output device
				&actualFeatureLevel,										   // Actual feature level (optional)
				GetD3D11COM<ID3D11DeviceContext**>(&context.api.d3d11.context) // Output device context
			);

			if (FAILED(result))
			{
				return false;
			}

			device.debug = debug;
			context.device = &device;

			return true;
		}

		void DestroyDevice(CGRenderDevice& device)
		{
			if (device.debug)
			{
				//{
				//	void*& dxgiInfoQueue = device.api.d3d11.dxgiInfoQueue;
				//	if (dxgiInfoQueue)
				//	{
				//		GetD3D11COM<IDXGIInfoQueue*>(dxgiInfoQueue)->Release();
				//		dxgiInfoQueue = nullptr;
				//	}

				//	void*& dxgiDebug = device.api.d3d11.dxgiDebug;
				//	if (dxgiDebug)
				//	{
				//		GetD3D11COM<IDXGIDebug*>(dxgiDebug)->Release();
				//		dxgiDebug = nullptr;
				//	}
				//}

				{
					void*& d3dInfoQueue = device.api.d3d11.d3dInfoQueue;
					if (d3dInfoQueue)
					{
						GetD3D11COM<ID3D11InfoQueue*>(d3dInfoQueue)->Release();
						d3dInfoQueue = nullptr;
					}

					void*& d3dDebug = device.api.d3d11.d3dDebug;
					if (d3dDebug)
					{
						GetD3D11COM<ID3D11Debug*>(d3dDebug)->Release();
						d3dDebug = nullptr;
					}
				}
			}

			void*& adapter = device.api.d3d11.adapter;
			if (adapter)
			{
				GetD3D11COM<IDXGIAdapter*>(adapter)->Release();
				adapter = nullptr;
			}

			void*& factory = device.api.d3d11.factory;
			if (factory)
			{
				GetD3D11COM<IDXGIFactory*>(factory)->Release();
				factory = nullptr;
			}

			void*& dev = device.api.d3d11.device;
			if (dev)
			{
				GetD3D11COM<ID3D11Device*>(dev)->Release();
				dev = nullptr;
			}
		}

		namespace DeviceOps
		{
			bool CreateAdapter(CGRenderDevice& device)
			{
				if (device.api.d3d11.adapter)
				{
					return false;
				}

				IDXGIDevice* dxgiDevice = nullptr;
				HRESULT result = GetD3D11COM<ID3D11Device*>(device.api.d3d11.device)->QueryInterface(IID_PPV_ARGS(&dxgiDevice));

				if (FAILED(result))
				{
					return false;
				}

				IDXGIAdapter* dxgiAdapter = nullptr;
				result = dxgiDevice->GetAdapter(&dxgiAdapter);
				dxgiDevice->Release();

				if (FAILED(result))
				{
					return false;
				}

				DXGI_ADAPTER_DESC desc = {};
				result = dxgiAdapter->GetDesc(&desc);
				dxgiAdapter->Release();

				if (FAILED(result))
				{
					return false;
				}

				char adapterName[128] = {};
				wcstombs(adapterName, desc.Description, sizeof(adapterName));

				device.deviceInfo.adapterName = adapterName;
				device.deviceInfo.dedicatedVideoMemory = desc.DedicatedVideoMemory / 1024 / 1024;
				device.deviceInfo.vendorId = desc.VendorId;
				device.api.d3d11.adapter = dxgiAdapter;

				printf("Direct3D 11 Context Info:\n\n");
				printf("  Physical Adapter: %s\n", device.deviceInfo.adapterName);
				printf("  Vendor Id: %u\n", device.deviceInfo.vendorId);
				printf("  Dedicated Video Memory: %lluMB\n", device.deviceInfo.dedicatedVideoMemory);

				return true;
			}

			bool CreateFactory(CGRenderDevice& device)
			{
				if (device.api.d3d11.factory)
				{
					return false;
				}

				IDXGIFactory* dxgiFactory = nullptr;
				HRESULT result = GetD3D11COM<IDXGIAdapter*>(device.api.d3d11.adapter)->GetParent(IID_PPV_ARGS(&dxgiFactory));

				if (FAILED(result))
				{
					return false;
				}

				device.api.d3d11.factory = dxgiFactory;

				return true;
			}

			bool CreateSwapchain(CGRenderContext& context, const core::CGWindow& window)
			{
				if (context.api.d3d11.swapchain)
				{
					return false;
				}

				context.api.d3d11.hwnd = window.nwh;

				DXGI_SWAP_CHAIN_DESC scd = {};
				scd.BufferDesc.Width = static_cast<UINT>(window.width);
				scd.BufferDesc.Height = static_cast<UINT>(window.height);
				scd.BufferDesc.RefreshRate = DXGI_RATIONAL{ 0U, 1U };
				scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				scd.SampleDesc = { 1U, 0U };
				scd.BufferCount = 2U;
				scd.OutputWindow = static_cast<HWND>(window.nwh);
				scd.Windowed = true;
				scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

				auto factory = GetD3D11COM<IDXGIFactory*>(context.device->api.d3d11.factory);
				auto device = GetD3D11COM<ID3D11Device*>(context.device->api.d3d11.device);

				HRESULT result = factory->CreateSwapChain(device, &scd, GetD3D11COM<IDXGISwapChain**>(&context.api.d3d11.swapchain));

				if (FAILED(result))
				{
					return false;
				}

				return true;
			}

			bool CreateRenderTargetView(const int8_t view, CGRenderContext& context)
			{
				if (view < 0 || view > context.api.d3d11.renderTargetViewCount ||
					context.api.d3d11.renderTargetViewCount + 1 > CG_MAX_RENDER_TARGET_VIEWS ||
					context.api.d3d11.renderTargetViews[view] != nullptr)
				{
					return false;
				}

				ID3D11Texture2D* backbuffer = nullptr;
				HRESULT result = GetD3D11COM<IDXGISwapChain*>(context.api.d3d11.swapchain)->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

				if (FAILED(result))
				{
					return false;
				}

				result = GetD3D11COM<ID3D11Device*>(context.device->api.d3d11.device)->CreateRenderTargetView(
					backbuffer,
					nullptr,
					GetD3D11COM<ID3D11RenderTargetView**>(&context.api.d3d11.renderTargetViews[view])
				);

				if (FAILED(result))
				{
					return false;
				}

				backbuffer->Release();
				context.api.d3d11.renderTargetViewCount++;

				return true;
			}

			bool CreateDebugInterface(CGRenderDevice& device)
			{
				if (device.api.d3d11.d3dDebug || device.api.d3d11.d3dInfoQueue)
				{
					return false;
				}

				{
					ID3D11Debug* d3dDebug = nullptr;
					ID3D11InfoQueue* d3dInfoQueue = nullptr;

					HRESULT result = GetD3D11COM<ID3D11Device*>(device.api.d3d11.device)->QueryInterface(IID_PPV_ARGS(&d3dDebug));

					if (FAILED(result))
					{
						return false;
					}

					result = d3dDebug->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue));

					if (FAILED(result))
					{
						d3dDebug->Release();

						return false;
					}

					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

					device.api.d3d11.d3dDebug = d3dDebug;
					device.api.d3d11.d3dInfoQueue = d3dInfoQueue;
				}

				//{
				//	IDXGIDebug* dxgiDebug = nullptr;
				//	IDXGIInfoQueue* dxgiInfoQueue = nullptr;

				//	HRESULT result = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

				//	if (FAILED(result))
				//	{
				//		return false;
				//	}

				//	result = dxgiDebug->QueryInterface(IID_PPV_ARGS(&dxgiInfoQueue));

				//	if (FAILED(result))
				//	{
				//		return false;
				//	}

				//	dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
				//	dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);

				//	device.api.d3d11.dxgiDebug = dxgiDebug;
				//	device.api.d3d11.dxgiInfoQueue = dxgiInfoQueue;
				//}

				return true;
			}
		}

		namespace ContextOps
		{
			void DestroyContext(CGRenderContext& context)
			{
				for (uint8_t i = 0; i < context.api.d3d11.renderTargetViewCount; ++i)
				{
					void*& renderTargetView = context.api.d3d11.renderTargetViews[i];
					if (renderTargetView)
					{
						GetD3D11COM<ID3D11RenderTargetView*>(renderTargetView)->Release();
						renderTargetView = nullptr;
					}
				}

				void*& swapchain = context.api.d3d11.swapchain;
				if (swapchain)
				{
					GetD3D11COM<IDXGIAdapter*>(swapchain)->Release();
					swapchain = nullptr;
				}

				void*& ctx = context.api.d3d11.context;
				if (ctx)
				{
					GetD3D11COM<ID3D11DeviceContext*>(ctx)->Release();
					ctx = nullptr;
				}
			}
		}

		namespace RenderOps
		{
			static void ClearView(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv, CGClearFlags clearFlags, float r, float g, float b, float a);

			void ExecuteRenderCommands(const CGRenderContext& context, const uint8_t count, const CGRenderCommand commands[])
			{
				if (count < 0 || count > CG_MAX_RENDER_COMMANDS)
				{
					return;
				}

				for (uint8_t i = 0; i < count; ++i)
				{
					const CGRenderCommand& command = commands[i];

					switch (command.commandType)
					{
						case CGRenderCommandType::None:
						{
							break;
						}
						case CGRenderCommandType::SetViewClear:
						{
							// This may cause unintended behaviour (no bounds check at run-time)
							void* renderTargetView = context.api.d3d11.renderTargetViews[command.params.setViewClear.view];
							uint32_t color = command.params.setViewClear.color;

							ClearView(
								GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
								GetD3D11COM<ID3D11RenderTargetView*>(renderTargetView),
								command.params.setViewClear.clearFlags,
								((color >> 24) & 0xFF) * CG_ONE_OVER_255,
								((color >> 16) & 0xFF) * CG_ONE_OVER_255,
								((color >> 8) & 0xFF) * CG_ONE_OVER_255,
								(color & 0xFF) * CG_ONE_OVER_255
							);

							continue;
						}
					}

					break;
				}
			}

			void ClearView(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv, CGClearFlags clearFlags, const float r, const float g, const float b, const float a)
			{
				if (!ctx || !rtv)
				{
					return;
				}

				ctx->OMSetRenderTargets(1, &rtv, nullptr);

				const float rgba[4] = { r, g, b, a };
				ctx->ClearRenderTargetView(rtv, rgba);
			}
		}

		namespace FrameOps
		{
			void Present(void* swapchain)
			{
				if (!swapchain)
				{
					return;
				}

				GetD3D11COM<IDXGISwapChain*>(swapchain)->Present(1, 0);
			}
		}

		namespace DebugOps
		{
			void ReportLiveObjects(CGRenderDevice& device)
			{
				if (!device.api.d3d11.d3dDebug /*|| !device.api.d3d11.dxgiDebug*/)
				{
					return;
				}

				const auto d3dDebug = GetD3D11COM<ID3D11Debug*>(device.api.d3d11.d3dDebug);
				d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);

				//const auto dxgiDebug = GetD3D11COM<IDXGIDebug*>(device.api.d3d11.dxgiDebug);
				//dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			}
		}
	}
}
