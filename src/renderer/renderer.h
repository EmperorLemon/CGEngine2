#pragma once

#include <cstdint>
#include <memory>
#include <functional>

#include "context.h"

// renderer.h
namespace cgengine::renderer
{
	enum class CGClearFlags : uint32_t
	{
		None = 1,
		Color = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2
	};

	struct CGRenderFunctions
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

		void SetRenderFunctions(const CGRenderFunctions& renderFunctions) { m_api = renderFunctions; }
	private:
		CGRenderFunctions m_api;
	};

	void SetupOpenGLFunctions(CGRenderer& renderer);
	void SetupD3D11Functions(CGRenderer& renderer);
}