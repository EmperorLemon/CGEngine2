#include <glad/gl.h>

#include "renderer.h"

// renderer_opengl.cpp
namespace cg::renderer::RenderOps::OpenGL
{
	static GLbitfield MapClearFlags(const CGClearFlags flags)
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

	void OpenGLClearView(const CGClearFlags flags, const float r, const float g, const float b, const float a)
	{
		glClearColor(r, g, b, a);
		glClear(MapClearFlags(flags));
	}

	static void BindVertexArray(const uint32_t vertexArray)
	{
		glBindVertexArray(vertexArray);
	}

	static void DrawArrays(const int32_t count)
	{
		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	static void DrawElements(const int32_t count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
	}

	static void Submit(const uint32_t shaderProgram)
	{
		glUseProgram(shaderProgram);
	}
}
