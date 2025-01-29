#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "types.h"
#include "context.h"
#include "platform/window.h"

// context_opengl.cpp

namespace cgengine::renderer
{
	void OpenGLInit(CGRenderContext& context)
	{
		context.Init = []()
			{
				return glfwInit();
			};

		context.Shutdown = []()
			{
				glfwTerminate();
			};
	}

	void CreateOpenGLContext(void* winptr, CGRenderContext& context)
	{
		auto window = static_cast<GLFWwindow*>(winptr);

		glfwMakeContextCurrent(window);

		if (!gladLoadGL(glfwGetProcAddress))
		{
			// glfwGetError(&error);
		}

		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

#ifdef _DEBUG
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
#endif

		context.SwapBuffers = [window]()
		{
			glfwSwapBuffers(window);
		};
	}
}