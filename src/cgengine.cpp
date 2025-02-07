#include "cgengine.h"

using namespace cg::renderer;

// cgengine.cpp
namespace cg
{
	static bool InitGraphicsAPI(const CGRendererType rendererType, const bool debug, CGRenderFunctions& function);
	static bool CreateWindow(const int32_t width, const int32_t height, core::CGWindow& window);
	static bool SetupGraphicsAPI(const CGEngineCreateInfo& info, const core::CGWindow& window, CGRenderer& renderer);

	CGEngine::CGEngine(const CGEngineCreateInfo& info)
	{
		m_renderer.type = info.rendererType;

		InitGraphicsAPI(info.rendererType, info.debug, m_renderer.functions);

		CreateWindow(static_cast<int32_t>(info.resolution.width), static_cast<int32_t>(info.resolution.height), m_window);

		SetupGraphicsAPI(info, m_window, m_renderer);
	}

	bool InitGraphicsAPI(const CGRendererType rendererType, [[maybe_unused]] const bool debug, CGRenderFunctions& functions)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				return D3D11::Init(functions);
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				//return renderer::OpenGLInit(renderContext, debug);
				break;
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return true;
	}

	bool CreateWindow(const int32_t width, const int32_t height, core::CGWindow& window)
	{
		return CreateCGWindow(width, height, "CGEngine Window", window);
	}

	bool SetupGraphicsAPI(const CGEngineCreateInfo& info, const core::CGWindow& window, CGRenderer& renderer)
	{
		switch (info.rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				if (!D3D11::CreateDevice(info.debug, renderer.device, renderer.context))
				{
					return false;
				}

				if (!D3D11::DeviceOps::CreateAdapter(renderer.device))
				{
					return false;
				}

				if (!D3D11::DeviceOps::CreateFactory(renderer.device))
				{
					return false;
				}

				if (!D3D11::DeviceOps::CreateSwapchain(window, renderer.context))
				{
					return false;
				}

				if (!D3D11::DeviceOps::CreateRenderTargetView(0, renderer.context))
				{
					return false;
				}

				if (info.debug)
				{
					if (!D3D11::DeviceOps::CreateDebugInterface(renderer.device))
					{
						return false;
					}
				}

				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				//return renderer::ContextOps::CreateOpenGLContext(window.winptr, renderContext, info.debug);
				break;
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
		m_renderer.functions.shutdown();

		switch (m_renderer.type)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				D3D11::DeviceOps::DestroyResources(m_renderer.resourcePool);

				D3D11::ContextOps::DestroyContext(m_renderer.context);

				D3D11::DestroyDevice(m_renderer.device);

				if (m_renderer.device.debug)
				{
					D3D11::DebugOps::ReportLiveObjects(m_renderer.device);
				}

				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				//renderer::ContextOps::DestroyOpenGLContext(m_renderContext);
				break; 
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}
	}
}
