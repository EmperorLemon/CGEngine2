#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <string>

#include "renderer.h"
#include "platform/window.h"

// renderer_d3d11.cpp
namespace cg::renderer::D3D11
{
	template <typename T>
	static constexpr T GetD3D11COM(void* ptr)
	{
		return reinterpret_cast<T>(ptr);
	}

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

	bool CreateDeviceAndContext(const bool debug, CGRenderDevice& device, CGRenderContext& context)
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
		constexpr static LPCSTR GetShaderTarget(const CGShaderType type)
		{
			switch (type)
			{
				case CGShaderType::None: break;
				case CGShaderType::Vertex: return "vs_5_0";
				case CGShaderType::Fragment: return "ps_5_0";
				case CGShaderType::TessellationControl: return "hs_5_0";
				case CGShaderType::TessellationEvaluation: return "ds_5_0";
				case CGShaderType::Geometry: return "gs_5_0";
				case CGShaderType::Compute: return "cs_5_0";
				case CGShaderType::Program: break;
			}

			return "NULL";
		}

		static bool CreateVertexShader(ID3D11Device* device, ID3DBlob* vsBlob, CGShader& vsShader);
		static bool CreatePixelShader(ID3D11Device* device, ID3DBlob* psBlob, CGShader& psShader);

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
			printf("  Debug: %s\n", device.debug ? "True" : "False");
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

		bool CreateSwapchain(const core::CGWindow& window, CGRenderContext& context)
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

		bool CreateRenderTargetView(const uint8_t view, CGRenderContext& context)
		{
			if (view > context.api.d3d11.renderTargetViewCount ||
				context.api.d3d11.renderTargetViewCount + 1u > CG_MAX_RENDER_TARGET_VIEWS ||
				context.api.d3d11.renderTargetViews[view] != nullptr)
			{
				return false;
			}

			ID3D11Texture2D* backbuffer = nullptr;
			HRESULT result = GetD3D11COM<IDXGISwapChain*>(context.api.d3d11.swapchain)->GetBuffer(0U, IID_PPV_ARGS(&backbuffer));

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

		bool CreateShader(const CGRenderContext& context, const CGShaderDesc& desc, CGShader& shader)
		{
			ID3DBlob* shaderBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;

			// too lazy to allocate memory for a wchar_t*
			std::wstring filename(desc.filename, desc.filename + strlen(desc.filename));

			LPCSTR target = GetShaderTarget(desc.shaderType);

			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

			if (context.device->debug)
			{
				flags |= D3DCOMPILE_DEBUG;
			}

			HRESULT result = D3DCompileFromFile(
				filename.c_str(),				   // .hlsl file
				nullptr,						   // defines
				D3D_COMPILE_STANDARD_FILE_INCLUDE, // include header
				desc.entryPoint,
				target,							   // target
				flags, 0U,						   // Flags1, Flags2
				&shaderBlob,
				&errorBlob
			);

			if (FAILED(result))
			{
				if (errorBlob)
				{
					printf(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
					errorBlob->Release();
				}

				if (shaderBlob)
				{
					shaderBlob->Release();
				}

				return false;
			}

			const auto device = GetD3D11COM<ID3D11Device*>(context.device->api.d3d11.device);

			bool valid = false;

			switch (desc.shaderType)
			{
				case CGShaderType::None:
				{
					break;
				}
				case CGShaderType::Vertex:
				{
					valid = CreateVertexShader(device, shaderBlob, shader);
					break;
				}
				case CGShaderType::Fragment:
				{
					valid = CreatePixelShader(device, shaderBlob, shader);
					break;
				}
			}

			if (errorBlob)
			{
				errorBlob->Release();
			}

			shader.api.d3d11.blob = shaderBlob;

			return valid;
		}

		bool CreateVertexShader(ID3D11Device* device, ID3DBlob* vsBlob, CGShader& vsShader)
		{
			HRESULT result = device->CreateVertexShader(
				vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(),
				nullptr,
				GetD3D11COM<ID3D11VertexShader**>(&vsShader.api.d3d11.shader)
			);

			if (FAILED(result))
			{
				return false;
			}

			return true;
		}

		bool CreateVertexLayout(const CGRenderDevice& device, CGShader& vShader, CGVertexLayout& vLayout)
		{
			const auto GetAttributeName = [](const CGVertexAttribute attribute)
			{
				switch (attribute)
				{
					case CGVertexAttribute::None:	  break;
					case CGVertexAttribute::Position: return "POSITION";
					case CGVertexAttribute::Color:	  return "COLOR";
					case CGVertexAttribute::Normal:   return "NORMAL";
					case CGVertexAttribute::TexCoord: return "TEXCOORD";
				}

				return "NONE";
			};

			const auto GetDXGIFormat = [](const CGVertexFormat format)
			{
				switch (format)
				{
					case CGVertexFormat::None:	 break;
					case CGVertexFormat::UInt:	 return DXGI_FORMAT_R32_UINT;
					case CGVertexFormat::UInt2:	 return DXGI_FORMAT_R32G32_UINT;
					case CGVertexFormat::UInt3:	 return DXGI_FORMAT_R32G32B32_UINT;
					case CGVertexFormat::UInt4:	 return DXGI_FORMAT_R32G32B32A32_UINT;
					case CGVertexFormat::Float:  return DXGI_FORMAT_R32_FLOAT;
					case CGVertexFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
					case CGVertexFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
					case CGVertexFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
				}

				return DXGI_FORMAT_UNKNOWN;
			};

			const auto dev = GetD3D11COM<ID3D11Device*>(device.api.d3d11.device);

			D3D11_INPUT_ELEMENT_DESC ied[CG_MAX_VERTEX_ELEMENTS] = {};

			for (uint8_t i = 0u; i < vLayout.count; ++i)
			{
				auto& element = vLayout.elements[i];

				ied[i].SemanticName = GetAttributeName(element.attribute);
				ied[i].SemanticIndex = 0U;
				ied[i].Format = GetDXGIFormat(element.format);
				ied[i].InputSlot = 0U;
				ied[i].AlignedByteOffset = element.offset;
				ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				ied[i].InstanceDataStepRate = 0U;
			}

			auto shaderBlob = GetD3D11COM<ID3DBlob*>(vShader.api.d3d11.blob);

			HRESULT result = dev->CreateInputLayout(
				ied, 
				vLayout.count, 
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				GetD3D11COM<ID3D11InputLayout**>(&vLayout.api.d3d11.layout)
			);

			shaderBlob->Release();
			shaderBlob = nullptr;

			if (FAILED(result))
			{
				return false;
			}

			return true;
		}

		bool CreateVertexBuffer(const CGRenderDevice& device, const CGBufferDesc& vbDesc, CGBuffer& vBuffer, const void* vbData)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = vbDesc.size;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0U;
			desc.MiscFlags = 0U;
			desc.StructureByteStride = 0U;

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = vbData;
			data.SysMemPitch = 0U;
			data.SysMemSlicePitch = 0U;

			const auto dev = GetD3D11COM<ID3D11Device*>(device.api.d3d11.device);
			HRESULT result = dev->CreateBuffer(&desc, &data, GetD3D11COM<ID3D11Buffer**>(&vBuffer.api.d3d11.buffer));

			if (FAILED(result))
			{
				return false;
			}

			return true;
		}

		bool CreatePixelShader(ID3D11Device* device, ID3DBlob* psBlob, CGShader& psShader)
		{
			HRESULT result = device->CreatePixelShader(
				psBlob->GetBufferPointer(),
				psBlob->GetBufferSize(),
				nullptr,
				GetD3D11COM<ID3D11PixelShader**>(&psShader.api.d3d11.shader)
			);

			if (FAILED(result))
			{
				return false;
			}

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

		void DestroyResources(CGResourcePool& resourcePool)
		{
			{
				CGBufferPool& bufferPool = resourcePool.bufferPool;

				for (uint8_t i = 0u; i < resourcePool.bufferPool.ibCount; ++i)
				{
					void*& indexBuffer = resourcePool.bufferPool.indexBuffers[i].api.d3d11.buffer;
					if (indexBuffer)
					{
						GetD3D11COM<ID3D11Buffer*>(indexBuffer)->Release();
						indexBuffer = nullptr;
					}
				}

				for (uint8_t i = 0u; i < resourcePool.bufferPool.vbCount; ++i)
				{
					void*& vertexBuffer = resourcePool.bufferPool.vertexBuffers[i].api.d3d11.buffer;
					if (vertexBuffer)
					{
						GetD3D11COM<ID3D11Buffer*>(vertexBuffer)->Release();
						vertexBuffer = nullptr;
					}
				}

				for (uint8_t i = 0u; i < bufferPool.vlCount; ++i)
				{
					void*& vertexLayout = bufferPool.vertexLayouts[i].api.d3d11.layout;
					if (vertexLayout)
					{
						GetD3D11COM<ID3D11InputLayout*>(vertexLayout)->Release();
						vertexLayout = nullptr;
					}
				}
			}

			{
				CGShaderPool& shaderPool = resourcePool.shaderPool;

				for (uint8_t i = 0u; i < shaderPool.fsCount; ++i)
				{
					void*& fShader = shaderPool.fragmentShaders[i].api.d3d11.shader;

					GetD3D11COM<ID3D11PixelShader*>(fShader)->Release();
					fShader = nullptr;
				}

				for (uint8_t i = 0u; i < shaderPool.vsCount; ++i)
				{
					void*& vShader = shaderPool.vertexShaders[i].api.d3d11.shader;

					GetD3D11COM<ID3D11VertexShader*>(vShader)->Release();
					vShader = nullptr;
				}
			}
		}
	}

	namespace RenderOps
	{
		static void Draw(ID3D11DeviceContext* ctx, const UINT count, const UINT start);
	}

	namespace ContextOps
	{
		static void OMSetClearView(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv, CGClearFlags clearFlags, float r, float g, float b, float a);
		static void RSSetViewport(ID3D11DeviceContext* ctx, const D3D11_VIEWPORT& viewport);
		static void IASetVertexBuffer(ID3D11DeviceContext* ctx, ID3D11InputLayout* vLayout, ID3D11Buffer* vBuffer, const UINT stride, const UINT offset);
		static void VSSetShader(ID3D11DeviceContext* ctx, ID3D11VertexShader* vShader);
		static void PSSetShader(ID3D11DeviceContext* ctx, ID3D11PixelShader* pShader);

		void OMSetClearView(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv, CGClearFlags clearFlags, const float r, const float g, const float b, const float a)
		{
			if (!ctx || !rtv)
			{
				return;
			}

			ctx->OMSetRenderTargets(1U, &rtv, nullptr);

			const FLOAT rgba[4] = { r, g, b, a };
			ctx->ClearRenderTargetView(rtv, rgba);
		}

		void RSSetViewport(ID3D11DeviceContext* ctx, const D3D11_VIEWPORT& viewport)
		{
			if (!ctx)
			{
				return;
			}

			ctx->RSSetViewports(1, &viewport);
		}

		void IASetVertexBuffer(ID3D11DeviceContext* ctx, ID3D11InputLayout* vLayout, ID3D11Buffer* vBuffer, const UINT stride, const UINT offset)
		{
			if (!ctx || !vLayout || !vBuffer)
			{
				return;
			}

			ctx->IASetInputLayout(vLayout);

			ctx->IASetVertexBuffers(0U, 1U, &vBuffer, &stride, &offset);

			ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		void VSSetShader(ID3D11DeviceContext* ctx, ID3D11VertexShader* vShader)
		{
			if (!ctx || !vShader)
			{
				return;
			}

			ctx->VSSetShader(vShader, nullptr, 0U);
		}

		void PSSetShader(ID3D11DeviceContext* ctx, ID3D11PixelShader* pShader)
		{
			if (!ctx || !pShader)
			{
				return;
			}

			ctx->PSSetShader(pShader, nullptr, 0U);
		}

		bool CreateViewport(const int32_t width, const int32_t height, CGViewport& viewport)
		{
			viewport.width = static_cast<float>(width);
			viewport.height = static_cast<float>(height);

			return true;
		}

		void ExecuteRenderCommands(const CGRenderContext& context, const CGResourcePool& resourcePool)
		{
			if (resourcePool.commandPool.count > CG_MAX_RENDER_COMMANDS)
			{
				return;
			}

			const CGBufferPool& bufferPool = resourcePool.bufferPool;
			const CGShaderPool& shaderPool = resourcePool.shaderPool;

			for (uint8_t i = 0u; i < resourcePool.commandPool.count; ++i)
			{
				const CGRenderCommand& cmd = resourcePool.commandPool.commands[i];

				switch (cmd.type)
				{
					case CGRenderCommandType::None:
					{
						break;
					}
					case CGRenderCommandType::SetViewClear:
					{
						void* renderTargetView = context.api.d3d11.renderTargetViews[cmd.params.setViewClear.view];
						uint32_t color = cmd.params.setViewClear.color;

						OMSetClearView(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							GetD3D11COM<ID3D11RenderTargetView*>(renderTargetView),
							cmd.params.setViewClear.clearFlags,
							((color >> 24) & 0xFF) * CG_ONE_OVER_255,
							((color >> 16) & 0xFF) * CG_ONE_OVER_255,
							((color >> 8) & 0xFF) * CG_ONE_OVER_255,
							(color & 0xFF) * CG_ONE_OVER_255
						);

						continue;
					}
					case CGRenderCommandType::SetViewport:
					{
						const CGViewport& viewport = context.api.d3d11.viewports[cmd.params.setViewport.viewport];

						D3D11_VIEWPORT vp = {};
						vp.Width = viewport.width;
						vp.Height = viewport.height;
						vp.MinDepth = viewport.minDepth;
						vp.MaxDepth = viewport.maxDepth;

						RSSetViewport(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							vp
						);

						continue;
					}
					case CGRenderCommandType::SetVertexShader:
					{
						void* vertexShader = shaderPool.vertexShaders[cmd.params.setShader.shader].api.d3d11.shader;

						VSSetShader(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							GetD3D11COM<ID3D11VertexShader*>(vertexShader)
						);

						continue;
					}
					case CGRenderCommandType::SetFragmentShader:
					{
						void* pixelShader = shaderPool.fragmentShaders[cmd.params.setShader.shader].api.d3d11.shader;

						PSSetShader(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							GetD3D11COM<ID3D11PixelShader*>(pixelShader)
						);

						continue;
					}
					case CGRenderCommandType::SetVertexBuffer:
					{
						const CGVertexLayout& vertexLayout = bufferPool.vertexLayouts[cmd.params.setVertexBuffer.buffer];
						const CGBuffer& vertexBuffer = bufferPool.vertexBuffers[cmd.params.setVertexBuffer.buffer];

						IASetVertexBuffer(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							GetD3D11COM<ID3D11InputLayout*>(vertexLayout.api.d3d11.layout),
							GetD3D11COM<ID3D11Buffer*>(vertexBuffer.api.d3d11.buffer),
							vertexBuffer.desc.stride,
							0U
						);

						continue;
					}
					case CGRenderCommandType::SetIndexBuffer:
					{
						continue;
					}
					case CGRenderCommandType::Draw:
					{
						RenderOps::Draw(
							GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
							cmd.params.draw.count,
							cmd.params.draw.start
						);

						continue;
					}
				}

				break;
			}
		}

		void DestroyContext(CGRenderContext& context)
		{
			for (uint8_t i = 0u; i < context.api.d3d11.renderTargetViewCount; ++i)
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
				GetD3D11COM<IDXGISwapChain*>(swapchain)->Release();
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
		void Draw(ID3D11DeviceContext* ctx, const UINT count, const UINT start)
		{
			if (!ctx)
			{
				return;
			}

			ctx->Draw(count, start);
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

			GetD3D11COM<IDXGISwapChain*>(swapchain)->Present(1U, 0U);
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
