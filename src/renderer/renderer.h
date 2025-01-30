#pragma once

#include <cstdint>
#include <memory>
#include <functional>

#include "context.hpp"

// renderer.h
namespace cg::renderer
{
	enum class CGClearFlags : uint32_t
	{
		None = 1,
		Color = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2
	};

	struct CGRenderAPIFunctions
	{
		std::function<void(CGClearFlags)> Clear;
		std::function<void(float, float, float, float)> ClearColor;
	};

	class CGRenderer
	{
	public:
		void Clear(CGClearFlags flags) const;
		void ClearColor(float r, float g, float b, float a = 1.0f) const;
		void ClearColor(uint32_t color) const;

		void SetRenderAPIFunctions(const CGRenderAPIFunctions& renderAPIFunctions) { m_api = renderAPIFunctions; }
	private:
		CGRenderAPIFunctions m_api;
	};

	void SetupOpenGLRenderFunctions(CGRenderer& renderer);
	void SetupD3D11RenderFunctions(CGRenderer& renderer);
}