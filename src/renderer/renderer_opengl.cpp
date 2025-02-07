#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "renderer.h"
#include "platform/window.h"

// renderer_opengl.cpp
namespace cg::renderer::OpenGL
{
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

	bool Init(const bool debug, CGRenderFunctions& functions)
	{
		functions.init = glfwInit;
		functions.shutdown = glfwTerminate;
	
		if (!functions.init())
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

	bool CreateDeviceAndContext(const bool debug, const core::CGWindow& window, CGRenderDevice& device, CGRenderContext& context)
	{
		auto winptr = static_cast<GLFWwindow*>(window.winptr);
	
		glfwMakeContextCurrent(winptr);
	
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

		context.api.opengl.window = window.winptr;

		//const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		device.deviceInfo.adapterName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		printf("OpenGL Context Info:\n\n");
		printf("  Debug: %s\n", device.debug ? "True" : "False");
		printf("  Physical Adapter: %s\n", device.deviceInfo.adapterName);
		printf("  OpenGL Version: %s\n", version);
		printf("  Shading Language Version: %s\n", shaderVersion);
	
		return true;
	}

	namespace DeviceOps
	{

	}

	namespace ContextOps
	{
		static constexpr GLbitfield MapClearFlags(const CGClearFlags flags)
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

		static void ClearView(const CGClearFlags flags, const float r, const float g, const float b, const float a)
		{
			glClearColor(r, g, b, a);
			glClear(MapClearFlags(flags));
		}

		static void BindVertexArray(const uint32_t vertexArray)
		{
			glBindVertexArray(vertexArray);
		}

		void ExecuteRenderCommands(const CGRenderContext& context, const CGResourcePool& resourcePool)
		{
			if (resourcePool.commandPool.count > CG_MAX_RENDER_COMMANDS)
			{
				return;
			}

			const CGBufferPool& bufferPool = resourcePool.bufferPool;
			const CGShaderPool& shaderPool = resourcePool.shaderPool;

			for (uint8_t i = 0u; i < resourcePool.commandPool.count; ++i)
			{
				const CGRenderCommand& cmd = resourcePool.commandPool.commands[i];

				switch (cmd.type)
				{
					case CGRenderCommandType::None:
					{
						break;
					}
					case CGRenderCommandType::SetViewClear:
					{
						uint32_t color = cmd.params.setViewClear.color;

						ClearView(
							cmd.params.setViewClear.clearFlags,
							((color >> 24) & 0xFF) * CG_ONE_OVER_255,
							((color >> 16) & 0xFF) * CG_ONE_OVER_255,
							((color >> 8) & 0xFF) * CG_ONE_OVER_255,
							(color & 0xFF) * CG_ONE_OVER_255
						);

						continue;
					}
					case CGRenderCommandType::SetViewport:
					{
						continue;
					}
					case CGRenderCommandType::SetVertexBuffer:
					{
						const CGBuffer& vertexBuffer = bufferPool.vertexBuffers[cmd.params.setVertexBuffer.vertexBuffer];

						BindVertexArray(vertexBuffer.api.opengl.buffer);

						continue;
					}
					case CGRenderCommandType::SetVertexShader:
					{
						continue;
					}
					case CGRenderCommandType::SetFragmentShader:
					{
						continue;
					}
					case CGRenderCommandType::Draw:
					{
						//RenderOps::Draw(
						//	GetD3D11COM<ID3D11DeviceContext*>(context.api.d3d11.context),
						//	cmd.params.draw.count,
						//	cmd.params.draw.start
						//);

						continue;
					}
				}

				break;
			}
		}
	}

	namespace FrameOps
	{
		void Present(void* window)
		{
			if (!window)
			{
				return;
			}

			glfwSwapBuffers(static_cast<GLFWwindow*>(window));
		}
	}

	//static void DrawArrays(const int32_t count)
	//{
	//	glDrawArrays(GL_TRIANGLES, 0, count);
	//}

	//void DrawElements(const int32_t count)
	//{
	//	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
	//}

	//void UseProgram(const uint32_t program)
	//{
	//	glUseProgram(program);
	//}
}
