#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "context.h"
#include "types.h"
#include "platform/window.h"

// context_opengl.cpp
namespace cg::renderer::ContextOps
{
	static void SwapBuffers(void* window);
	static void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties);

	static void APIENTRY DebugMessageCallback(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, [[maybe_unused]] const GLsizei length, const GLchar* message, [[maybe_unused]] const void* userData)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		const auto src_str = [source]
		{
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:			  return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:	  return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:	  return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION:	  return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER:			  return "OTHER";
			default: break;
			}

			return "UNKNOWN SOURCE";
		}();

		const auto type_str = [type]
		{
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:				return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOUR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED BEHAVIOUR";
			case GL_DEBUG_TYPE_PORTABILITY:		    return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE:			return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:				return "MARKER";
			case GL_DEBUG_TYPE_OTHER:				return "OTHER";
			default: break;
			}

			return "UNKNOWN TYPE";
		}();

		const auto severity_str = [severity]
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW:			 return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM:		 return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH:		 return "HIGH";
			default: break;
			}

			return "UNKNOWN SEVERITY";
		}();

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			printf("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			printf("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			printf("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			printf("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
			break;
		default: break;
		}
	}

	bool OpenGLInit(CGRenderContext& renderContext, const bool debug)
	{
		renderContext.apiFunctions.init = glfwInit;
		renderContext.apiFunctions.shutdown = glfwTerminate;

		if (!renderContext.apiFunctions.init())
		{
			return false;
		}

		// Set GLFW window hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug);

		return true;
	}

	bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext, const bool debug)
	{
		auto window = static_cast<GLFWwindow*>(winptr);

		if (!window)
		{
			return false;
		}

		glfwMakeContextCurrent(window);

		if (!gladLoadGL(glfwGetProcAddress))
		{
			const char* error;
			glfwGetError(&error);

			printf("GLFW/OpenGL Error: %s", error);

			return false;
		}

		GLint flags; 
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (debug && ((flags & GL_CONTEXT_FLAG_DEBUG_BIT)))
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(DebugMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		QueryPhysicalDevice(renderContext.deviceProperties);
		SetupOpenGLContextFunctions(renderContext);

		return true;
	}

	void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties)
	{
		const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		deviceProperties.renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		//glGetIntegerv(GL_MAJOR_VERSION, &deviceProperties.versionMajor);
		//glGetIntegerv(GL_MINOR_VERSION, &deviceProperties.versionMinor);

		GLint numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		printf("OpenGL Context Info:\n\n");

		printf(
			"Vendor: %s\n"
			"Renderer: %s\n"
			"OpenGL Version: %s\n"
			"GLSL Version: %s",
			vendor, deviceProperties.renderer,
			version, shaderVersion
		);

		for (GLint i = 0; i < numExtensions; ++i)
		{
			const char* extensionName = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
		}
	}

	static void SwapBuffers(void* window)
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(window));
	}

	void SetupOpenGLContextFunctions(CGRenderContext& renderContext)
	{
		renderContext.apiFunctions.present = SwapBuffers;
	}

	void DestroyOpenGLContext(CGRenderContext& renderContext)
	{
		//DestroyOpenGLBuffers(renderContext.bufferPool);
		//DestroyOpenGLShaderPrograms();
	}
}
