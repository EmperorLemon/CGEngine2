#pragma once

#include <functional>
#include <memory>

#include "types.h"

namespace cg::core { struct CGWindow; }

// context.h
namespace cg::renderer
{
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

	class CGRenderContext
	{
	public:
		CGRenderContext() = default;
		~CGRenderContext() = default;

		std::function<bool()> Init = nullptr;
		std::function<void()> Shutdown = nullptr;
		std::function<void()> BeginFrame = nullptr;
		std::function<void()> RenderFrame = nullptr;
		std::function<void()> EndFrame = nullptr;
		std::function<void()> Present = nullptr;
		
		template <CGRendererType Type>
		CGRenderContextData<Type>* GetData();

		template <CGRendererType Type>
		const CGRenderContextData<Type>* GetData() const;

		bool GetDebug() const { return m_debug; }

		void SetData(std::unique_ptr<CGRenderContextDataBase>&& data) { m_data = std::move(data); }
		void SetDebug(const bool debug) { m_debug = debug; }
	private:
		std::unique_ptr<CGRenderContextDataBase> m_data;
		bool m_debug = false;
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

	void D3D11Init(CGRenderContext& renderContext);
	void OpenGLInit(CGRenderContext& renderContext);

	bool CreateD3D11Context(const CGSwapchainConfig& swapchainConfig, void* nativeWindowHandle, CGRenderContext& renderContext);
	bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext);

	void SetupD3D11ContextFunctions(void* nativeWindowHandle, CGRenderContext& renderContext);
	void SetupOpenGLContextFunctions(void* winptr, CGRenderContext& renderContext);
}