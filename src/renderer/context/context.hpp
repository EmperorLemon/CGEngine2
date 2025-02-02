#pragma once

#include <string_view>

#include "types.h"
#include "../buffer/buffer.h"
#include "../shader/shader.h"

namespace cg::core { struct CGWindow; }

// context.h
namespace cg::renderer
{
	struct CGDeviceProperties
	{
		std::string_view vendor;
		std::string_view renderer;
		std::string_view version;
		std::string_view glslVersion;
		int32_t versionMajor = 0;
		int32_t versionMinor = 0;
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
		CGRenderContextAPIFunctions apiFunctions;
		CGDeviceProperties deviceProperties;
	};

	// Context operations
	namespace ContextOps
	{
		//bool D3D11Init(CGRenderContext& renderContext);
		bool OpenGLInit(CGRenderContext& renderContext, bool debug);

		//bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext, bool debug);
		bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext, bool debug);

		//void SetupD3D11ContextFunctions(void* nativeWindowHandle, CGRenderContext& renderContext);
		void SetupOpenGLContextFunctions(CGRenderContext& renderContext);
	}

	// Frame operations
	namespace FrameOps
	{
		void Present(const CGRenderContextAPIFunctions& apiFunctions, void* window);
	}

}


