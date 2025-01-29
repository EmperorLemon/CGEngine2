#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "window.h"
#include "types.h"

// window_glfw.cpp

namespace cgengine
{
	bool CreateCGWindow(const CGRendererType type, const bool debug, int32_t width, int32_t height, const char* title, core::CGWindow& window)
	{
		if (type == CGRendererType::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug);
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		window.width = width;
		window.height = height;
		window.winptr = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window.winptr)
		{
			const char* error = nullptr;
			glfwGetError(&error);

			return false;
		}

#ifdef _WIN32
		window.nwh = glfwGetWin32Window(static_cast<GLFWwindow*>(window.winptr));
#endif

		if (!window.nwh)
		{
			const char* error = nullptr;
			glfwGetError(&error);

			return false;
		}

		return true;
	}

	void DestroyCGWindow(void* winptr)
	{
		glfwDestroyWindow(static_cast<GLFWwindow*>(winptr));
	}

	void PollEvents()
	{
		glfwPollEvents();
	}

	bool ShouldCloseCGWindow(void* winptr)
	{
		return glfwWindowShouldClose(static_cast<GLFWwindow*>(winptr));
	}
}