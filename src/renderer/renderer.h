#pragma once

#include "context.hpp"

constexpr uint32_t CG_CLEAR_COLOR = 0x1;
constexpr uint32_t CG_CLEAR_DEPTH = 0x1 << 1;
constexpr uint32_t CG_CLEAR_STENCIL = 0x1 << 2;

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

		void SetRenderAPIFunctions(const CGRenderAPIFunctions& renderAPIFunctions) { m_api = renderAPIFunctions; }
	private:
		CGRenderAPIFunctions m_api;
	};

	void SetupOpenGLRenderFunctions(CGRenderer& renderer);
	void SetupD3D11RenderFunctions(CGRenderer& renderer, const CGRenderContext& renderContext);
}