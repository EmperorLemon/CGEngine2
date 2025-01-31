#pragma once

#include "types.h"
#include "platform/window.h"
#include "renderer/renderer.h"

// cgengine.h
namespace cg
{
	struct CGResolution
	{
		uint32_t width = 0u;
		uint32_t height = 0u;
	};

	struct CGEngineCreateInfo
	{
		CGRendererType rendererType = CGRendererType::None;
		CGResolution resolution;
		bool debug = false;
	};

	class CGEngine
	{
	public:
		CGEngine(const CGEngineCreateInfo& info);
		~CGEngine();

		bool IsRunning() const;

		const renderer::CGRenderContext& GetRenderContext() const { return m_renderContext; }
		const renderer::CGRenderer& GetRenderer() const { return m_renderer; }
	private:
		renderer::CGRenderContext m_renderContext;
		renderer::CGRenderer m_renderer;
		core::CGWindow m_window;
	};
}