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
		None = 0u,
		Direct3D11 = 1u,
		Direct3D12 = 2u,
		OpenGL = 3u,
		Vulkan = 4u,
	};
}

// renderer.h
namespace cg::renderer
{
	/* ----Constants---- */
#pragma region Constants

	constexpr float CG_ONE_OVER_255 = 0.003921568627451f;
	constexpr uint8_t CG_MAX_RENDER_TARGET_VIEWS = 8u;
	constexpr uint8_t CG_MAX_VIEWPORTS = 8u;
	constexpr uint8_t CG_MAX_VERTEX_ELEMENTS = 8u;
	constexpr uint8_t CG_MAX_VERTEX_LAYOUTS = 8u;
	constexpr uint8_t CG_MAX_VERTEX_BUFFERS = 128u;
	constexpr uint8_t CG_MAX_INDEX_BUFFERS = 128u;
	constexpr uint8_t CG_MAX_VERTEX_SHADERS = 32u;
	constexpr uint8_t CG_MAX_FRAGMENT_SHADERS = 32u;
	constexpr uint8_t CG_MAX_SHADER_PROGRAMS = 64u;
	constexpr uint8_t CG_MAX_RENDER_COMMANDS = 128u;

#pragma endregion

	/* ----Enums---- */
#pragma region Enums

	enum class CGVertexAttribute : uint32_t
	{
		None = 0u,
		Position = 1u,
		Color = 2u,
		Normal = 3u,
		TexCoord = 4u,
	};

	enum class CGVertexFormat : uint32_t
	{
		None = 0u,
		Float = 1u,
		Float2 = 2u,
		Float3 = 3u,
		Float4 = 4u,
		UInt = 5u,
		UInt2 = 6u,
		UInt3 = 7u,
		UInt4 = 8u
	};

	enum class CGBufferType : uint16_t
	{
		None = 0u,
		Vertex = 1u,
		Index = 2u,
		Constant = 3u
	};

	enum class CGBufferUsage : uint16_t
	{
		None = 0u,
		Static = 1u,
		Dynamic = 2u
	};

	enum class CGShaderType : uint8_t
	{
		None = 0u,
		Vertex = 1u,
		Fragment = 2u,
		TessellationControl = 3u,
		TessellationEvaluation = 4u,
		Geometry = 5u,
		Compute = 6u,
		Program = 7u
	};

	enum class CGRenderCommandType : uint32_t
	{
		None = 0u,
		SetViewClear = 1u,
		SetViewport = 2u,
		SetVertexBuffer = 3u,
		SetVertexShader = 4u,
		SetIndexBuffer = 5u,
		SetFragmentShader = 6u,
		Draw = 7u,
		DrawIndexed = 8u,
		Submit = 9u
	};

	enum CGColor : uint32_t
	{
		CG_BLACK = 0x000000FF,
		CG_WHITE = 0xFFFFFFFF,
		CG_RED = 0xFF0000FF,
		CG_GREEN = 0x00FF00FF,
		CG_BLUE = 0x0000FFFF
	};

	enum CGClearFlags : uint16_t
	{
		CG_CLEAR_COLOR = 1 << 0,
		CG_CLEAR_DEPTH = 1 << 1,
		CG_CLEAR_STENCIL = 1 << 2
	};

#pragma endregion

	/* ----Data Structures---- */
#pragma region Data Structures

	struct CGRenderFunctions
	{
		int(*init)() = nullptr;
		void(*shutdown)() = nullptr;
	};

	struct CGViewport
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;
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
				void* device;		   // ID3D11Device*
				void* factory;		   // IDXGIFactory*
				void* adapter;		   // IDXGIAdapter*
				void* d3dDebug;		   // ID3D11Debug*
				void* d3dInfoQueue;    // ID3D11InfoQueue*
				//void* dxgiDebug;	   // IDXGIDebug*
				//void* dxgiInfoQueue; // IDXGIInfoQueue*
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

	struct alignas(16) CGVertexElement
	{
		CGVertexAttribute attribute = CGVertexAttribute::None;
		CGVertexFormat format = CGVertexFormat::None;
		uint32_t offset = 0u;
		uint32_t size = 0u;
	};

	struct alignas(16) CGVertexLayout
	{
		CGVertexElement elements[CG_MAX_VERTEX_ELEMENTS] = {};

		union
		{
			struct {
				void* layout;
				uint32_t padding[2];
			} d3d11;
		} api = {};

		uint32_t padding[2] = {};
		uint32_t count = 0u;
		uint32_t size = 0u;
	};

	struct alignas(16) CGBufferDesc
	{
		uint32_t count = 0u;
		uint32_t stride = 0u;
		uint32_t size = 0u;
		CGBufferType type = CGBufferType::None;
		CGBufferUsage usage = CGBufferUsage::None;
	};

	struct alignas(16) CGBuffer
	{
		union
		{
			struct
			{
				void* buffer = nullptr; // ID3D11Buffer*
				uint32_t padding[2];
			} d3d11;
		} api = {};

		CGBufferDesc desc = {};
	};

	struct CGShaderDesc
	{
		const wchar_t* filename = nullptr;
		const char* entryPoint = nullptr;
		CGShaderType shaderType = CGShaderType::None;
	};

	struct CGShader
	{
		union
		{
			struct
			{
				void* blob = nullptr;   // ID3DBlob*
				void* shader = nullptr; // ID3D11<Type>Shader*
			} d3d11;
		} api = {};

		CGShaderType type = CGShaderType::None;
	};

	struct alignas(16) CGRenderCommand
	{
		union {
			struct {
				uint32_t color;
				CGClearFlags clearFlags;
				uint8_t view;
			} setViewClear;
			struct
			{
				uint8_t viewport;
			} setViewport;
			struct {
				uint8_t shader;
			} setShader;
			struct {
				uint32_t vertexBuffer;
			} setVertexBuffer;
			struct {
				uint32_t count;
				uint32_t start;
				uint8_t vertexBuffer;
			} draw;
			struct {
				uint32_t count;
			} drawIndexed;
			struct {
				uint32_t program;
				uint8_t view;
			} submit;
		} params = {};

		CGRenderCommandType type = CGRenderCommandType::None;
	};

	struct alignas(16) CGBufferPool
	{
		CGBuffer vertexBuffers[CG_MAX_VERTEX_BUFFERS] = {};
		CGBuffer indexBuffers[CG_MAX_INDEX_BUFFERS] = {};

		CGVertexLayout vertexLayouts[CG_MAX_VERTEX_LAYOUTS] = {};

		uint32_t vbCount = 0u; // Vertex buffer count
		uint32_t ibCount = 0u; // Index buffer count
		uint32_t vlCount = 0u; // Vertex layout count
		uint32_t padding = 0u;
	};

	struct CGShaderPool
	{
		CGShader vertexShaders[CG_MAX_VERTEX_SHADERS] = {};
		CGShader fragmentShaders[CG_MAX_FRAGMENT_SHADERS] = {};

		uint8_t vsCount = 0u;
		uint8_t fsCount = 0u;
	};

	struct CGCommandPool
	{
		CGRenderCommand commands[CG_MAX_RENDER_COMMANDS] = {};
		uint8_t count = 0u;
	};

	struct CGResourcePool
	{
		CGBufferPool bufferPool = {};
		CGShaderPool shaderPool = {};
		CGCommandPool commandPool = {};
	};

	struct CGRenderer
	{
		CGResourcePool resourcePool = {};
		CGRenderContext context = {};
		CGRenderDevice device = {};
		CGRenderFunctions functions = {}; // why?
		CGRendererType type = CGRendererType::None;
	};

#pragma endregion

	/* ----Function Declarations---- */
#pragma region Function Declarations

	bool AddRenderCommands(const uint8_t count, const CGRenderCommand commands[], CGCommandPool& commandPool);
	void ExecuteRenderCommands(const CGRenderer& renderer);

	namespace DeviceOps
	{
		bool CreateShader(const CGShaderDesc& desc, CGRenderer& renderer, CGShader& shader);
		bool CreateVertexLayout(const uint8_t count, CGVertexElement elements[], CGRenderer& renderer, CGShader& vShader, CGVertexLayout& vLayout);
		bool CreateVertexBuffer(const CGBufferDesc& vbDesc, CGRenderer& renderer, const void* vbData);
	}

	namespace ContextOps
	{
		bool CreateViewport(const core::CGWindow& window, CGRenderer& renderer);

		CGRenderCommand SetViewClear(const uint8_t view, const CGClearFlags flags, const uint32_t color);
		CGRenderCommand SetViewport(const uint8_t viewport);
		CGRenderCommand SetVertexShader(const uint8_t vertexShader);
		CGRenderCommand SetVertexBuffer(const uint32_t vertexBuffer);
		CGRenderCommand SetFragmentShader(const uint8_t fragmentShader);
	}

	namespace RenderOps
	{
		CGRenderCommand Draw(const uint8_t vertexBuffer, const uint32_t count, const uint32_t start);
		CGRenderCommand DrawIndexed(const uint32_t count);
		CGRenderCommand Submit(const uint8_t view, const uint32_t program);
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
		bool CreateDevice(const bool debug, CGRenderDevice& device, CGRenderContext& context);
		void DestroyDevice(CGRenderDevice& device);

		namespace DeviceOps
		{
			bool CreateAdapter(CGRenderDevice& device);
			bool CreateFactory(CGRenderDevice& device);
			bool CreateSwapchain(const core::CGWindow& window, CGRenderContext& context);
			bool CreateRenderTargetView(const uint8_t view, CGRenderContext& context);
			bool CreateShader(const CGRenderContext& context, const CGShaderDesc& desc, CGShader& shader);
			bool CreateVertexLayout(const CGRenderDevice& device, CGShader& vShader, CGVertexLayout& layout);
			bool CreateVertexBuffer(const CGRenderDevice& device, const CGBufferDesc& vbDesc, CGBuffer& vbBuffer, const void* vbData);
			bool CreateDebugInterface(CGRenderDevice& device);
			void DestroyResources(CGResourcePool& resourcePool);
		}

		namespace ContextOps
		{
			bool CreateViewport(const int32_t width, const int32_t height, CGViewport& viewport);
			void ExecuteRenderCommands(const CGRenderContext& context, const CGResourcePool& resourcePool);
			void DestroyContext(CGRenderContext& context);
		}

		namespace RenderOps
		{
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
