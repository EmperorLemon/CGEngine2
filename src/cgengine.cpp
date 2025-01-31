#include "cgengine.h"

#include "core/logger.hpp"

// cgengine.cpp
namespace cg
{
	static bool InitGraphicsAPI(CGRendererType rendererType, renderer::CGRenderContext& renderContext, bool debug);
	static bool CreateWindow(int32_t width, int32_t height, core::CGWindow& window);
	static bool CreateRenderContext(const CGEngineCreateInfo& info, core::CGWindow& window, renderer::CGRenderContext& renderContext);
	static void SetupRenderFunctions(CGRendererType rendererType, renderer::CGRenderer& renderer, renderer::CGRenderContext& renderContext);

	CGEngine::CGEngine(const CGEngineCreateInfo& info)
	{
		core::CGLogger::Init();

		InitGraphicsAPI(info.rendererType, m_renderContext, info.debug);

		CreateWindow(info.resolution.width, info.resolution.height, m_window);

		CreateRenderContext(info, m_window, m_renderContext);

		SetupRenderFunctions(info.rendererType, m_renderer, m_renderContext);
	}

	bool InitGraphicsAPI(const CGRendererType rendererType, renderer::CGRenderContext& renderContext, const bool debug)
	{
		switch (rendererType)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
		{
			if (!renderer::D3D11Init(renderContext))
			{
				return false;
			}

			break;
		}
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
		{
			if (!renderer::OpenGLInit(renderContext, debug))
			{
				return false;
			}

			break;
		}
		case CGRendererType::Vulkan:
			break;
		}

		return true;
	}

	bool CreateWindow(int32_t width, int32_t height, core::CGWindow& window)
	{
		if (!CreateCGWindow(width, height, "CGEngine Window", window))
		{
			return false;
		}

		return true;
	}

	bool CreateRenderContext(const CGEngineCreateInfo& info, core::CGWindow& window, renderer::CGRenderContext& renderContext)
	{
		renderer::CGSwapchainConfig swapchainConfig;
		swapchainConfig.bufferCount = 2;
		swapchainConfig.width = info.resolution.width;
		swapchainConfig.height = info.resolution.height;
		swapchainConfig.vsync = true;
		swapchainConfig.windowed = true;

		switch (info.rendererType)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
		{
			if (!renderer::CreateD3D11Context(swapchainConfig, window.nwh, renderContext, info.debug))
			{
				return false;
			}

			break;
		}
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
		{
			if (!renderer::CreateOpenGLContext(window.winptr, renderContext, info.debug))
			{
				return false;
			}

			break;
		}
		case CGRendererType::Vulkan:
			break;
		}

		return true;
	}

	void SetupRenderFunctions(const CGRendererType rendererType, renderer::CGRenderer& renderer, renderer::CGRenderContext& renderContext)
	{
		switch (rendererType)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
		{
			renderer::SetupD3D11RenderFunctions(renderer, renderContext);
			break;
		}
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
		{
			renderer::SetupOpenGLRenderFunctions(renderer);
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
		m_renderContext.Shutdown();
	}
}