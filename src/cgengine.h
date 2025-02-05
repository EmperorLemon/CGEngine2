#pragma once

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

		const renderer::CGRenderer& GetRenderer() const { return m_renderer; }
		renderer::CGRenderer& GetRenderer() { return m_renderer; }

		const core::CGWindow& GetWindow() const { return m_window; }
		core::CGWindow& GetWindow() { return m_window; }
	private:
		renderer::CGRenderer m_renderer;
		core::CGWindow m_window;
	};
}
