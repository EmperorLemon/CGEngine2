#include "cgengine.h"

#include "core/logger.hpp"

// cgengine.cpp
namespace cg
{
	static constexpr int32_t cast(uint32_t value) { return static_cast<int32_t>(value); }

	CGEngine::CGEngine(const CGEngineCreateInfo& info)
	{
		core::CGLogger::Init();

		switch (info.type)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
			renderer::D3D11Init(m_context);
			break;
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
			renderer::OpenGLInit(m_context);
			break;
		case CGRendererType::Vulkan:
			break;
		}

		if (!m_context.Init())
		{
			// failure
			return;
		}

		if (!CreateCGWindow(info.type, info.debug, cast(info.resolution.width), cast(info.resolution.height), "CGEngine Window", m_window))
		{
			// failure
			return;
		}

		m_context.SetDebug(info.debug);

		renderer::CGSwapchainConfig swapchainConfig;
		swapchainConfig.bufferCount = 2;
		swapchainConfig.width = info.resolution.width;
		swapchainConfig.height = info.resolution.height;
		swapchainConfig.vsync = true;
		swapchainConfig.windowed = true;

		switch (info.type)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
		{
			if (!renderer::CreateD3D11Context(swapchainConfig, m_window.nwh, m_context))
			{
				break;
			}

			SetupD3D11RenderFunctions(m_renderer);

			break;
		}
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
		{
			if (!renderer::CreateOpenGLContext(m_window.winptr, m_context))
			{
				break;
			}

			SetupOpenGLRenderFunctions(m_renderer);

			break;
		}
		case CGRendererType::Vulkan:
			break;
		}
	}

	bool CGEngine::IsRunning() const
	{
		return !ShouldCloseCGWindow(m_window.winptr);
	}

	CGEngine::~CGEngine()
	{
		m_context.Shutdown();
	}
}