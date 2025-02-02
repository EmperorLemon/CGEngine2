#include "renderer.h"

// renderer.cpp
namespace cg::renderer
{
	constexpr float ONE_OVER_255 = 0.003921568627451f;

	void CGRenderer::ClearView(const uint32_t flags, const float r, const float g, const float b, const float a) const
	{
		if (renderAPIFunctions.clearColor)
		{
			renderAPIFunctions.clearColor(flags, r, g, b, a);
		}
	}

	void CGRenderer::ClearView(const uint32_t flags, const uint32_t color) const
	{
		float r = ((color >> 24) & 0xFF) * ONE_OVER_255;
		float g = ((color >> 16) & 0xFF) * ONE_OVER_255;
		float b = ((color >> 8) & 0xFF) * ONE_OVER_255;
		float a = (color & 0xFF) * ONE_OVER_255;

		if (renderAPIFunctions.clearColor)
		{
			renderAPIFunctions.clearColor(flags, r, g, b, a);
		}
	}

	void CGRenderer::BindVertexBuffer(const uint32_t vertexBuffer) const
	{
		if (renderAPIFunctions.bindVertexBuffer)
		{
			renderAPIFunctions.bindVertexBuffer(vertexBuffer);
		}
	}

	void CGRenderer::DrawArrays(const int32_t count) const
	{
		if (renderAPIFunctions.drawArrays)
		{
			renderAPIFunctions.drawArrays(count);
		}
	}

	void CGRenderer::DrawElements(const int32_t count) const
	{
		if (renderAPIFunctions.drawElements)
		{
			renderAPIFunctions.drawElements(count);
		}
	}

	void CGRenderer::Submit(const uint32_t shaderProgram) const
	{
		if (renderAPIFunctions.submit)
		{
			renderAPIFunctions.submit(shaderProgram);
		}
	}
}
