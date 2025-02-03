#pragma once

#include "types.h"
#include "../buffer/buffer.h"
#include "../shader/shader.h"

namespace cg::core { struct CGWindow; }

// context.h
namespace cg::renderer
{
	constexpr uint8_t CG_MAX_RENDER_TARGET_VIEWS = 8;

	struct CGD3D11Context
	{
		void* renderTargets[CG_MAX_RENDER_TARGET_VIEWS]; // ID3D11RenderTargetView*
		void* device = nullptr;							 // ID3D11Device*
		void* deviceContext = nullptr;					 // ID3D11DeviceContext*
		void* swapchain = nullptr;						 // IDXGISwapChain*
		uint32_t renderTargetCount = 0;
	};

	struct CGPhysicalDeviceProperties
	{
		const char* renderer = nullptr;
		size_t dedicatedVideoMemory = 0;
		size_t dedicatedSystemMemory = 0;
	};

	struct CGSwapchainConfig
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t bufferCount = 0;
		bool vsync = false;
		bool windowed = false;
	};

	struct CGRenderContextAPIFunctions
	{
		int(*init)() = nullptr;
		void(*shutdown)() = nullptr;
		void(*beginFrame)() = nullptr;
		void(*renderFrame)() = nullptr;
		void(*endFrame)() = nullptr;
		void(*present)(void*) = nullptr;
	};

	struct CGRenderContext
	{
		CGVIBufferPool bufferPool;
		CGShaderHandlePool shaderPool;

		union
		{
			CGD3D11Context d3d11Context = {};
		};

		CGRenderContextAPIFunctions apiFunctions;
		CGPhysicalDeviceProperties deviceProperties;
		CGRendererType rendererType = CGRendererType::None;
	};

	// Context operations
	namespace ContextOps
	{
		bool D3D11Init(CGRenderContext& renderContext);
		bool OpenGLInit(CGRenderContext& renderContext, bool debug);

		bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext, bool debug);
		bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext, bool debug);

		void SetupD3D11ContextFunctions(CGRenderContext& renderContext);
		void SetupOpenGLContextFunctions(CGRenderContext& renderContext);

		void DestroyD3D11Context(CGRenderContext& renderContext);
		void DestroyOpenGLContext(CGRenderContext& renderContext);
	}

	// Frame operations
	namespace FrameOps
	{
		void Present(const CGRenderContext& renderContext, const core::CGWindow& window);
	}

}


