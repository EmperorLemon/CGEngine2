#include <glad/gl.h>

#include "renderer.h"

// renderer_opengl.cpp
namespace cg::renderer
{
	static GLbitfield MapClearFlags(const uint32_t flags)
	{
		GLbitfield glFlags = 0u;

		if (flags & CG_CLEAR_COLOR)
		{
			glFlags |= GL_COLOR_BUFFER_BIT;
		}

		if (flags & CG_CLEAR_DEPTH)
		{
			glFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (flags & CG_CLEAR_STENCIL)
		{
			glFlags |= GL_STENCIL_BUFFER_BIT;
		}

		return glFlags;
	}

	void SetupOpenGLRenderFunctions(CGRenderer& renderer)
	{
		CGRenderAPIFunctions renderAPIFunctions;

		renderAPIFunctions.ClearColor = [](const uint32_t flags, const float r, const float g, const float b, const float a)
		{
			glClearColor(r, g, b, a);
			glClear(MapClearFlags(flags));
		};

		renderer.SetRenderAPIFunctions(renderAPIFunctions);
	}
}