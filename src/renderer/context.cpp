#include "context.hpp"

// context.cpp
namespace cg::renderer
{
	bool CGRenderContext::Init()
	{
		if (m_renderContextAPIFunctions.init)
		{
			return m_renderContextAPIFunctions.init();
		}

		return false;
	}

	void CGRenderContext::Shutdown()
	{
		if (m_renderContextAPIFunctions.shutdown)
		{
			m_renderContextAPIFunctions.shutdown();
		}
	}

	void CGRenderContext::BeginFrame() const
	{
		if (m_renderContextAPIFunctions.beginFrame)
		{
			m_renderContextAPIFunctions.beginFrame();
		}
	}

	void CGRenderContext::RenderFrame() const
	{
		if (m_renderContextAPIFunctions.renderFrame)
		{
			m_renderContextAPIFunctions.renderFrame();
		}
	}

	void CGRenderContext::EndFrame() const
	{
		if (m_renderContextAPIFunctions.endFrame)
		{
			m_renderContextAPIFunctions.endFrame();
		}
	}

	void CGRenderContext::Present() const
	{
		if (m_renderContextAPIFunctions.present)
		{
			m_renderContextAPIFunctions.present();
		}
	}
}