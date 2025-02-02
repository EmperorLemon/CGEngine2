#pragma once

#include "defines.h"
#include "context/context.hpp"

// renderer.h
namespace cg::renderer
{
	struct CGRenderAPIFunctions
	{
		void (*clearColor)(uint32_t, float, float, float, float) = nullptr;
		void (*bindVertexBuffer)(uint32_t) = nullptr;
		void (*drawArrays)(int32_t count) = nullptr;
		void (*drawElements)(int32_t count) = nullptr;
		void (*submit)(uint32_t) = nullptr;
	};

	struct CGRenderer
	{
		CGRenderAPIFunctions renderAPIFunctions;

		void ClearView(uint32_t flags, float r, float g, float b, float a = 1.0f) const;
		void ClearView(uint32_t flags, uint32_t color) const;
		void BindVertexBuffer(uint32_t vertexBuffer) const;
		void DrawArrays(int32_t count) const;
		void DrawElements(int32_t count) const;
		void Submit(uint32_t shaderProgram) const;
	};

	void SetupOpenGLRenderFunctions(CGRenderer& renderer);
	//void SetupD3D11RenderFunctions(CGRenderer& renderer, const CGRenderContext& renderContext);
}
