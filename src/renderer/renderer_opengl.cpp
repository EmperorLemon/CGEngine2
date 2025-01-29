#include <glad/gl.h>
#include <iostream>

#include "renderer.h"

// renderer_opengl.cpp

namespace cgengine::renderer
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

	void SetupOpenGLFunctions(CGRenderer& renderer)
	{
		renderer.api.Clear = [](const CGClearFlags flags)
			{
				glClear(MapClearFlags(flags));
			};

		renderer.api.ClearColor = [](const float r, const float g, const float b, const float a)
			{
				glClearColor(r, g, b, a);
			};
	}

	//void ClearColor(uint32_t color)
	//{
	//	float r = ((color >> 24) & 0xFF) / 255.0f;
	//	float g = ((color >> 16) & 0xFF) / 255.0f;
	//	float b = ((color >> 8) & 0xFF) / 255.0f;
	//	float a = (color & 0xFF) / 255.0f;

	//	glClearColor(r, g, b, a);
	//}
}