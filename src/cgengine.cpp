#include "cgengine.h"

// cgengine.cpp
namespace cg
{
	static bool InitGraphicsAPI(CGRendererType rendererType, renderer::CGRenderContext& renderContext, bool debug);
	static bool CreateWindow(int32_t width, int32_t height, core::CGWindow& window);
	static bool CreateRenderContext(const CGEngineCreateInfo& info, core::CGWindow& window, renderer::CGRenderContext& renderContext);

	CGEngine::CGEngine(const CGEngineCreateInfo& info) : m_renderer(m_renderContext)
	{
		m_renderContext.rendererType = info.rendererType;

		InitGraphicsAPI(info.rendererType, m_renderContext, info.debug);

		CreateWindow(static_cast<int32_t>(info.resolution.width), static_cast<int32_t>(info.resolution.height), m_window);

		CreateRenderContext(info, m_window, m_renderContext);
	}

	bool InitGraphicsAPI(const CGRendererType rendererType, renderer::CGRenderContext& renderContext, const bool debug)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				return renderer::ContextOps::D3D11Init(renderContext);
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				return renderer::ContextOps::OpenGLInit(renderContext, debug);
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return true;
	}

	bool CreateWindow(int32_t width, int32_t height, core::CGWindow& window)
	{
		return CreateCGWindow(width, height, "CGEngine Window", window);
	}

	bool CreateRenderContext(const CGEngineCreateInfo& info, core::CGWindow& window, renderer::CGRenderContext& renderContext)
	{
		renderer::CGSwapchainConfig swapchainConfig = {};
		swapchainConfig.bufferCount = 2;
		swapchainConfig.width = info.resolution.width;
		swapchainConfig.height = info.resolution.height;
		swapchainConfig.vsync = true;
		swapchainConfig.windowed = true;

		switch (info.rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				return renderer::ContextOps::CreateD3D11Context(swapchainConfig, window.nwh, renderContext, info.debug);
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				return renderer::ContextOps::CreateOpenGLContext(window.winptr, renderContext, info.debug);
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return true;
	}

	bool CGEngine::IsRunning() const
	{
		return !ShouldCloseCGWindow(m_window.winptr);
	}

	CGEngine::~CGEngine()
	{
		m_renderContext.apiFunctions.shutdown();

		switch (m_renderContext.rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				renderer::ContextOps::DestroyD3D11Context(m_renderContext);
				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				renderer::ContextOps::DestroyOpenGLContext(m_renderContext);
				break; 
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		
	}
}
