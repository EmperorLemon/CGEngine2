#include <GLFW/glfw3.h>
#include <cstdio>

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

#include "window.h"
#include "types.h"

// window_glfw.cpp
namespace cg
{
	bool CreateCGWindow(int32_t width, int32_t height, const char* title, core::CGWindow& window)
	{
		window.width = width;
		window.height = height;
		window.winptr = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window.winptr)
		{
			const char* error = nullptr;
			glfwGetError(&error);

			printf("GLFW Error: %s", error);

			return false;
		}

#ifdef _WIN32
		window.nwh = glfwGetWin32Window(static_cast<GLFWwindow*>(window.winptr));
#endif

		if (!window.nwh)
		{
			const char* error = nullptr;
			glfwGetError(&error);

			printf("GLFW Error: %s", error);

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
