#pragma once

#include <functional>
#include <memory>
#include <string_view>

#include "types.h"

namespace cg::core { struct CGWindow; }

// context.h
namespace cg::renderer
{
	struct CGPhysicalDeviceProperties
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

	struct CGRenderContextDataBase
	{
		virtual ~CGRenderContextDataBase() = default;
	};

	// Base template for renderer-specific data
	template <CGRendererType Type>
	struct CGRenderContextData : CGRenderContextDataBase {};

	struct CGRenderContextAPIFunctions
	{
		std::function<bool()> init = nullptr;
		std::function<void()> shutdown = nullptr;
		std::function<void()> beginFrame = nullptr;
		std::function<void()> renderFrame = nullptr;
		std::function<void()> endFrame = nullptr;
		std::function<void()> present = nullptr;
	};

	class CGRenderContext
	{
	public:
		CGRenderContext() = default;
		~CGRenderContext() = default;

	public:
		bool Init();
		void Shutdown();
		void BeginFrame() const;
		void RenderFrame() const;
		void EndFrame() const;
		void Present() const;

	public:
		template <CGRendererType Type>
		CGRenderContextData<Type>* GetData();

		template <CGRendererType Type>
		const CGRenderContextData<Type>* GetData() const;

		void SetData(std::unique_ptr<CGRenderContextDataBase>&& data) { m_data = std::move(data); }

		bool GetDebug() const { return m_debug; }
		void SetDebug(const bool debug) { m_debug = debug; }

		const CGPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_physicalDeviceProperties; }
		void SetPhysicalDeviceProperties(const CGPhysicalDeviceProperties& deviceProperties) { m_physicalDeviceProperties = deviceProperties; }

		const CGRenderContextAPIFunctions& GetRenderContextAPIFunctions() const { return m_renderContextAPIFunctions; }
		CGRenderContextAPIFunctions& GetRenderContextAPIFunctions() { return m_renderContextAPIFunctions; }
		void SetRenderContextAPIFunctions(const CGRenderContextAPIFunctions& apiFunctions) { m_renderContextAPIFunctions = apiFunctions; }

	private:
		std::unique_ptr<CGRenderContextDataBase> m_data;
		bool m_debug = false;
		CGPhysicalDeviceProperties m_physicalDeviceProperties;
		CGRenderContextAPIFunctions m_renderContextAPIFunctions;
	};

	template <CGRendererType Type>
	CGRenderContextData<Type>* CGRenderContext::GetData()
	{
		return dynamic_cast<CGRenderContextData<Type>*>(m_data.get());
	}

	template <CGRendererType Type>
	const CGRenderContextData<Type>* CGRenderContext::GetData() const
	{
		return dynamic_cast<CGRenderContextData<Type>*>(m_data.get());
	}

	bool D3D11Init(CGRenderContext& renderContext);
	bool OpenGLInit(CGRendererType rendererType, CGRenderContext& renderContext);

	bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext);
	bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext);

	void SetupD3D11ContextFunctions(void* nativeWindowHandle, CGRenderContext& renderContext);
	void SetupOpenGLContextFunctions(void* winptr, CGRenderContext& renderContext);
}