#pragma once

#include "defines.h"
#include "context.hpp"

// renderer.h
namespace cg::renderer
{
	struct CGRenderAPIFunctions
	{
		std::function<void(uint32_t, float, float, float, float)> ClearColor;
	};

	class CGRenderer
	{
	public:
		void ClearView(uint32_t flags, float r, float g, float b, float a = 1.0f) const;
		void ClearView(uint32_t flags, uint32_t color) const;

		const CGRenderAPIFunctions& GetRenderAPIFunctions() const { return m_renderAPIFunctions; }
		CGRenderAPIFunctions& GetAPIFunctions() { return m_renderAPIFunctions; }
		void SetRenderAPIFunctions(const CGRenderAPIFunctions& renderAPIFunctions) { m_renderAPIFunctions = renderAPIFunctions; }
	private:
		CGRenderAPIFunctions m_renderAPIFunctions;
	};

	void SetupOpenGLRenderFunctions(CGRenderer& renderer);
	void SetupD3D11RenderFunctions(CGRenderer& renderer, const CGRenderContext& renderContext);
}