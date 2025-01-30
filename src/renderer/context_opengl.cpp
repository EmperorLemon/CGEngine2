#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/logger.hpp"
#include "context.hpp"
#include "types.h"
#include "platform/window.h"

// context_opengl.cpp
namespace cg::renderer
{
	static void APIENTRY DebugMessageCallback(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei length, const GLchar* message, const void* userData)
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
			core::CG_TRACE("{0}, {1}, {2}, {3}: {4}", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			core::CG_WARN("{0}, {1}, {2}, {3}: {4}", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			core::CG_ERROR("{0}, {1}, {2}, {3}: {4}", src_str, type_str, severity_str, id, message);
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			core::CG_CRITICAL("{0}, {1}, {2}, {3}: {4}", src_str, type_str, severity_str, id, message);
			break;
		default: break;
		}
	}

	void OpenGLInit(CGRenderContext& renderContext)
	{
		renderContext.Init = []()
			{
				return glfwInit();
			};

		renderContext.Shutdown = []()
			{
				glfwTerminate();
			};
	}

	bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext)
	{
		auto window = static_cast<GLFWwindow*>(winptr);

		if (!window)
			return false;

		glfwMakeContextCurrent(window);

		if (!gladLoadGL(glfwGetProcAddress))
		{
			// glfwGetError(&error);
			return false;
		}

		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

#ifdef _DEBUG
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(DebugMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#else
		if (renderContext.GetDebug() && ((flags & GL_CONTEXT_FLAG_DEBUG_BIT)))
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(DebugMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif

		SetupOpenGLContextFunctions(window, renderContext);

		return true;
	}

	void SetupOpenGLContextFunctions(void* winptr, CGRenderContext& renderContext)
	{
		auto window = static_cast<GLFWwindow*>(winptr);

		renderContext.Present = [window]()
			{
				glfwSwapBuffers(window);
			};
	}
}