#pragma once

#include "types.h"
#include "platform/window.h"
#include "renderer/renderer.h"

// cgengine.h
namespace cgengine
{
	struct CGResolution
	{
		uint32_t width = 0u;
		uint32_t height = 0u;
	};

	struct CGEngineCreateInfo
	{
		CGRendererType type = CGRendererType::None;
		CGResolution resolution;
		bool debug = false;
	};

	class CGEngine
	{
	public:
		CGEngine(const CGEngineCreateInfo& info);
		~CGEngine();

		bool IsRunning() const;

		const renderer::CGRenderContext& GetContext() const { return m_context; }
		const renderer::CGRenderer& GetRenderer() const { return m_renderer; }
	private:
		core::CGWindow m_window;
		renderer::CGRenderContext m_context;
		renderer::CGRenderer m_renderer;
	};
}