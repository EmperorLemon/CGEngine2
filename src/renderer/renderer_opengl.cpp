#include <glad/gl.h>
#include <iostream>

#include "renderer.h"

// renderer_opengl.cpp
namespace cg::renderer
{
	static GLbitfield MapClearFlags(const CGClearFlags flags)
	{
		GLbitfield glFlags = 0u;

		if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(CGClearFlags::Color))
			glFlags |= GL_COLOR_BUFFER_BIT;

		if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(CGClearFlags::Depth))
			glFlags |= GL_DEPTH_BUFFER_BIT;

		if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(CGClearFlags::Stencil))
			glFlags |= GL_STENCIL_BUFFER_BIT;

		return glFlags;
	}

	void SetupOpenGLRenderFunctions(CGRenderer& renderer)
	{
		CGRenderAPIFunctions renderAPIFunctions;

		renderAPIFunctions.Clear = [](const CGClearFlags flags)
			{
				glClear(MapClearFlags(flags));
			};

		renderAPIFunctions.ClearColor = [](const float r, const float g, const float b, const float a)
			{
				glClearColor(r, g, b, a);
			};

		renderer.SetRenderAPIFunctions(renderAPIFunctions);
	}
}