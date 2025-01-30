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

	bool OpenGLInit(CGRendererType rendererType, CGRenderContext& renderContext)
	{
		auto& renderContextAPIFunctions = renderContext.GetRenderContextAPIFunctions();

		renderContextAPIFunctions.init = []()
		{
			return glfwInit();
		};

		renderContextAPIFunctions.shutdown = []()
		{
			glfwTerminate();
		};

		if (!renderContext.Init())
		{
			return false;
		}

		// Set GLFW window hints
		if (rendererType == CGRendererType::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, renderContext.GetDebug());
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		return true;
	}

	static void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties);

	bool CreateOpenGLContext(void* winptr, CGRenderContext& renderContext)
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

			core::CG_ERROR("GLFW/OpenGL Error: {0}", error);

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
		CGPhysicalDeviceProperties deviceProperties;

		QueryPhysicalDevice(deviceProperties);
		SetupOpenGLContextFunctions(window, renderContext);

		renderContext.SetPhysicalDeviceProperties(deviceProperties);

		return true;
	}

	void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties)
	{
		deviceProperties.vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		deviceProperties.renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		deviceProperties.version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		deviceProperties.glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		glGetIntegerv(GL_MAJOR_VERSION, &deviceProperties.versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &deviceProperties.versionMinor);

		GLint numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		core::CG_INFO("\n"
			"Vendor: {0}\n"
			"Renderer: {1}\n"
			"OpenGL Version: {2}\n"
			"GLSL Version: {3}",
			deviceProperties.vendor, deviceProperties.renderer,
			deviceProperties.version, deviceProperties.glslVersion
		);

		core::CG_TRACE("\nNumber of extensions available: {0}\n", numExtensions);

		for (GLint i = 0; i < numExtensions; ++i)
		{
			const std::string_view extensionName = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));

			core::CG_TRACE(" {0}", extensionName);
		}
	}

	void SetupOpenGLContextFunctions(void* winptr, CGRenderContext& renderContext)
	{
		auto window = static_cast<GLFWwindow*>(winptr);

		auto& renderContextAPIFunctions = renderContext.GetRenderContextAPIFunctions();

		renderContextAPIFunctions.present = [window]()
		{
			glfwSwapBuffers(window);
		};
	}
}