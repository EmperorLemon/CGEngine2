#include "cgengine.h"

#include "renderer/renderer.h"

// cgengine.cpp
namespace cgengine
{
	static constexpr int32_t cast(uint32_t value) { return static_cast<int32_t>(value); }

	CGEngine::CGEngine(const CGEngineCreateInfo& info)
	{
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

		switch (info.type)
		{
		case CGRendererType::None:
			break;
		case CGRendererType::Direct3D11:
			renderer::CreateD3D11Context(m_context);
			SetupD3D11Functions(m_renderer);
			break;
		case CGRendererType::Direct3D12:
			break;
		case CGRendererType::OpenGL:
			renderer::CreateOpenGLContext(m_window.winptr, m_context);
			SetupOpenGLFunctions(m_renderer);
			break;
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