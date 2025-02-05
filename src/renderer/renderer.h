#pragma once

#include <cstdint>

namespace cg::core 
{ 
	struct CGWindow; 
}

namespace cg
{
	enum class CGRendererType : uint8_t
	{
		None = 0,
		Direct3D11 = 1,
		Direct3D12 = 2,
		OpenGL = 3,
		Vulkan = 4,
	};
}

// renderer.h
namespace cg::renderer
{
	/* ----Constants---- */
#pragma region Constants

	constexpr float CG_ONE_OVER_255 = 0.003921568627451f;
	constexpr uint8_t  CG_MAX_RENDER_TARGET_VIEWS = 8;
	constexpr uint8_t  CG_MAX_VIEWPORTS = 8;
	constexpr uint8_t  CG_MAX_VERTEX_ELEMENTS = 16;
	constexpr uint16_t CG_MAX_VERTEX_BUFFERS = 512;
	constexpr uint16_t CG_MAX_INDEX_BUFFERS = 512;
	constexpr uint16_t CG_MAX_SHADERS = 512;
	constexpr uint8_t  CG_MAX_SHADER_PROGRAMS = 64;
	constexpr uint8_t  CG_MAX_RENDER_COMMANDS = 128;

#pragma endregion

	/* ----Enums---- */
#pragma region Enums

	enum class CGVertexAttribute : uint8_t
	{
		None = 0,
		Position = 1,
		Normal = 2,
		TexCoord0 = 3,
	};

	enum class CGDataType : uint8_t
	{
		None = 0,
		UInt8 = 1,
		UInt16 = 2,
		UInt32 = 3,
		Float = 4,
		Float2 = 5,
		Float3 = 6,
		Float4 = 7
	};

	enum class CGShaderType : uint8_t
	{
		None = 0,
		Vertex = 1,
		Fragment = 2,
		Geometry = 3,
		TessellationControl = 4,
		TessellationEvaluation = 5,
		Compute = 6,
		Program = 7,
	};

	enum class CGRenderCommandType : uint32_t
	{
		None = 0,
		SetViewClear = 1,
		SetVertexBuffer = 2,
		SetIndexBuffer = 3,
		DrawVertices = 4,
		DrawIndexed = 5,
		Submit = 6
	};

	enum CGColor : uint32_t
	{
		CG_BLACK = 0x000000FF,
		CG_WHITE = 0xFFFFFFFF,
		CG_RED = 0xFF0000FF,
		CG_GREEN = 0x00FF00FF,
		CG_BLUE = 0x0000FFFF,
	};

	enum CGClearFlags : uint16_t
	{
		CG_CLEAR_COLOR = 0x1,
		CG_CLEAR_DEPTH = 0x1 << 1,
		CG_CLEAR_STENCIL = 0x1 << 2
	};

#pragma endregion

	/* ----Data Structures---- */
#pragma region Data Structures

	struct CGRenderFunctions
	{
		int(*init)() = nullptr;
		void(*shutdown)() = nullptr;
		void(*present)() = nullptr;
	};

	struct CGViewport
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 0.0f;
	};

	struct CGSwapchainDesc
	{
		uint32_t width = 0u;
		uint32_t height = 0u;
		bool windowed = false;
	};

	struct CGPhysicalDeviceInfo
	{
		const char* adapterName = nullptr;
		uint64_t dedicatedVideoMemory = 0ull;
		uint32_t vendorId = 0u;
		bool isDiscrete = false;
	};

	// Handles resource creation and device-level operations
	struct CGRenderDevice
	{
		// API-specific device handles
		union
		{
			struct
			{
				void* device;		 // ID3D11Device*
				void* factory;		 // IDXGIFactory*
				void* adapter;		 // IDXGIAdapter*
				void* d3dDebug;		 // ID3D11Debug*
				void* d3dInfoQueue;  // ID3D11InfoQueue*
			} d3d11;
		} api = {};

		CGPhysicalDeviceInfo deviceInfo = {};
		bool debug = false;
	};

	// Handles rendering commands and state
	struct CGRenderContext
	{
		union {
			struct {
				void* context;										 // ID3D11DeviceContext*
				void* swapchain;									 // IDXGISwapChain*
				void* hwnd;											 // HWND - Handle to a *Win32* window
				void* renderTargetViews[CG_MAX_RENDER_TARGET_VIEWS]; // ID3D11RenderTargetView*
				CGViewport viewports[CG_MAX_VIEWPORTS];				 // D3D_VIEWPORT
				uint8_t renderTargetViewCount;
				uint8_t viewportCount;
			} d3d11;
		} api = {};

		const CGRenderDevice* device = nullptr;
	};

	struct alignas(16) CGRenderCommand
	{
		CGRenderCommandType commandType = CGRenderCommandType::None;
		uint8_t padding[4] = {};

		union {
			struct {
				uint32_t color; // 4 bytes
				CGClearFlags clearFlags; // 2 bytes
				int8_t view; // 1 byte
			} setViewClear;
			struct {
				uint32_t vertexBuffer;
			} setVertexBuffer;
			struct {
				int32_t count;
			} drawVertices;
			struct {
				int32_t count;
			} drawIndexed;
			struct {
				uint32_t program;
				int8_t view;
			} submit;
		} params = {};
	};

	struct CGCommandList
	{
		CGRenderCommand commands[CG_MAX_RENDER_COMMANDS] = {};
		int8_t count = 0;
	};

	struct CGRenderer
	{
		CGCommandList commandList = {};
		CGRenderContext context = {};
		CGRenderDevice device = {};
		CGRenderFunctions functions = {};
		CGRendererType type = CGRendererType::None;
	};

#pragma endregion

	/* ----Function Declarations---- */
#pragma region Function Declarations

	bool AddRenderCommands(CGRenderer& renderer, int8_t count, const CGRenderCommand commands[]);
	void ExecuteRenderCommands(const CGRenderer& renderer);

	namespace RenderOps
	{
		CGRenderCommand SetViewClear(int8_t view, CGClearFlags flags, uint32_t color);
		CGRenderCommand SetVertexBuffer(uint32_t vertexBuffer);
		CGRenderCommand DrawIndexed(int32_t count);
		CGRenderCommand Submit(int8_t view, uint32_t program);
	}

	namespace FrameOps
	{
		void Present(const CGRenderer& renderer);
	}

	namespace OpenGL
	{
		//namespace RenderOps
		//{
		//	void ClearView(CGClearFlags flags, float r, float g, float b, float a);
		//	void BindVertexArray(uint32_t vertexArray);
		//	void DrawElements(int32_t count);
		//	void UseProgram(uint32_t program);
		//}
	}

	namespace D3D11
	{
		bool Init(CGRenderFunctions& functions);
		bool CreateDevice(CGRenderDevice& device, CGRenderContext& context, bool debug);
		void DestroyDevice(CGRenderDevice& device);

		namespace DeviceOps
		{
			bool CreateAdapter(CGRenderDevice& device);
			bool CreateFactory(CGRenderDevice& device);
			bool CreateSwapchain(CGRenderContext& context, const core::CGWindow& window);
			bool CreateRenderTargetView(int8_t view, CGRenderContext& context);
			bool CreateDebugInterface(CGRenderDevice& device);
		}

		namespace ContextOps
		{
			void DestroyContext(CGRenderContext& context);
		}

		namespace RenderOps
		{
			void ExecuteRenderCommands(const CGRenderContext& context, uint8_t count, const CGRenderCommand commands[]);
		}

		namespace FrameOps
		{
			void Present(void* swapchain);
		}

		namespace DebugOps
		{
			// Useful for identifying memory leaks
			void ReportLiveObjects(CGRenderDevice& device);
		}
	}

#pragma endregion

	//inline constexpr int32_t GetDataTypeSize(const CGDataType dataType)
	//{
	//	switch (dataType)
	//	{
	//	case CGDataType::None:	  return -1;
	//	case CGDataType::UInt8:   return sizeof(uint8_t);
	//	case CGDataType::UInt16:  return sizeof(uint16_t);
	//	case CGDataType::UInt32:  return sizeof(uint32_t);
	//	case CGDataType::Float:   return sizeof(float);
	//	case CGDataType::Float2:  return 2 * sizeof(float);
	//	case CGDataType::Float3:  return 3 * sizeof(float);
	//	case CGDataType::Float4:  return 4 * sizeof(float);
	//	}

	//	return -1;
	//}

	//inline constexpr int32_t GetByteStride(const CGVertexLayout& layout)
	//{
	//	int32_t byteStride = 0;

	//	for (uint8_t i = 0; i < layout.elementCount; ++i)
	//	{
	//		const int32_t dataSize = GetDataTypeSize(layout.vertexElements[i].dataType);
	//		byteStride += dataSize;
	//	}

	//	return byteStride;
	//}

	//inline constexpr int32_t GetAttributeCount(const CGDataType dataType)
	//{
	//	switch (dataType)
	//	{
	//	case CGDataType::None:   return -1;
	//	case CGDataType::UInt8:  return 1;
	//	case CGDataType::UInt16: return 1;
	//	case CGDataType::UInt32: return 1;
	//	case CGDataType::Float:  return 1;
	//	case CGDataType::Float2: return 2;
	//	case CGDataType::Float3: return 3;
	//	case CGDataType::Float4: return 4;
	//	default: break;
	//	}

	//	return -1;
	//}

	//inline constexpr void GetAttributeOffset(CGVertexLayout& layout)
	//{
	//	uint32_t offset = 0u;

	//	for (uint8_t i = 0; i < layout.elementCount; ++i)
	//	{
	//		const int32_t size = GetDataTypeSize(layout.vertexElements[i].dataType);

	//		if (i > 0)
	//		{
	//			offset += static_cast<uint32_t>(size);
	//			layout.vertexElements[i].offset = offset;
	//		}
	//	}
	//}
}
