#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "renderer.h"
#include "platform/window.h"
#include "io/fileio.h"

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
		static constexpr GLenum GetShaderType(const CGShaderType shaderType)
		{
			switch (shaderType)
			{
				case CGShaderType::None:				   return ~0u;
				case CGShaderType::Vertex:				   return GL_VERTEX_SHADER;
				case CGShaderType::Fragment:			   return GL_FRAGMENT_SHADER;
				case CGShaderType::Geometry:			   return GL_GEOMETRY_SHADER;
				case CGShaderType::TessellationControl:    return GL_TESS_CONTROL_SHADER;
				case CGShaderType::TessellationEvaluation: return GL_TESS_EVALUATION_SHADER;
				case CGShaderType::Compute:				   return GL_COMPUTE_SHADER;
				case CGShaderType::Program:				   return ~0u;
			}
		
			return ~0u;
		}

		static constexpr bool CheckShaderCompileErrors(const GLuint shader, const CGShaderType shaderType)
		{
			GLint  success = 0;
			GLchar infoLog[1024] = {};
		
			const auto shaderTypeStr = [shaderType]
			{
				switch (shaderType)
				{
					case CGShaderType::None:				   return "Unknown shader";
					case CGShaderType::Vertex:				   return "Vertex shader";
					case CGShaderType::Fragment:			   return "Fragment shader";
					case CGShaderType::Geometry:			   return "Geometry shader";
					case CGShaderType::TessellationControl:    return "Tessellation control shader";
					case CGShaderType::TessellationEvaluation: return "Tessellation evaluation shader";
					case CGShaderType::Compute:				   return "Compute shader";
					case CGShaderType::Program:				   return "Shader program";
				}
		
				return "Unknown shader";
			}();
		
			if (shaderType != CGShaderType::Program)
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
					printf("Error::SHADER_COMPILATION_ERROR of type: %s\n%s", shaderTypeStr, infoLog);
					return false;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
		
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
					printf("Error::PROGRAM_LINKING_ERROR of type: %s\n%s", shaderTypeStr, infoLog);
					return false;
				}
			}

			return true;
		}

		bool CreateShader(const CGShaderDesc& desc, CGShader& shader)
		{				
			io::CGFile shaderFile = io::ReadFile(desc.filename);
			char* shaderSource = shaderFile.data.get();

			uint32_t& _shader = shader.api.opengl.shader;
			
			_shader = glCreateShader(GetShaderType(shader.type));
			glShaderSource(_shader, 1, &shaderSource, nullptr);
			glCompileShader(_shader);
			
			
			if (!CheckShaderCompileErrors(_shader, shader.type))
			{
				glDeleteShader(_shader);
				return false;
			}

			return true;
		}

		bool CreateShaderProgram(const uint8_t shaderCount, const CGShader shaders[], uint32_t& program)
		{
			program = glCreateProgram();

			// Attach shaders
			for (uint8_t i = 0u; i < shaderCount; ++i)
			{
				const uint32_t shader = shaders[i].api.opengl.shader;
				glAttachShader(program, shader);
			}

			glLinkProgram(program);

			const auto DeleteShaders = [shaderCount, &shaders, program]()
			{
				for (uint8_t i = 0u; i < shaderCount; ++i)
				{
					const uint32_t shader = shaders[i].api.opengl.shader;

					glDetachShader(program, shader);
					glDeleteShader(shader);
				}
			};

			if (!CheckShaderCompileErrors(program, CGShaderType::Program))
			{
				DeleteShaders();
				glDeleteProgram(program);

				return false;
			}

			DeleteShaders();

			return true;
		}

		bool CreateVertexBuffer(const CGBufferDesc& vbDesc, CGBuffer& vBuffer, const void* vbData)
		{
			uint32_t& buffer = vBuffer.api.opengl.buffer;

			glCreateBuffers(1, &buffer);

			if (buffer == 0)
			{
				return false;
			}

			glNamedBufferStorage(buffer, static_cast<GLsizeiptr>(vbDesc.size), vbData, 0u);
			
			if (glGetError() != GL_NO_ERROR)
			{
				glDeleteBuffers(1, &buffer);
				return false;
			}

			return true;
		}

		bool CreateIndexBuffer(const CGBufferDesc& ibDesc, CGBuffer& iBuffer, const void* ibData)
		{
			uint32_t& buffer = iBuffer.api.opengl.buffer;

			glCreateBuffers(1, &buffer);

			if (buffer == 0u)
			{
				return false;
			}

			glNamedBufferStorage(buffer, static_cast<GLsizeiptr>(ibDesc.size), ibData, 0u);

			if (glGetError() != GL_NO_ERROR)
			{
				glDeleteBuffers(1, &buffer);
				return false;
			}

			return true;
		}

		static constexpr GLint GetAttributeCount(const CGVertexFormat format)
		{
			switch (format)
			{
				case CGVertexFormat::None:   return -1;
				case CGVertexFormat::UInt:   return 1;
				case CGVertexFormat::UInt2:  return 2;
				case CGVertexFormat::UInt3:  return 3;
				case CGVertexFormat::UInt4:  return 4;
				case CGVertexFormat::Float:  return 1;
				case CGVertexFormat::Float2: return 2;
				case CGVertexFormat::Float3: return 3;
				case CGVertexFormat::Float4: return 4;
			}

			return -1;
		}

		static constexpr GLenum GetAttributeType(const CGVertexFormat format)
		{
			switch (format)
			{
				case CGVertexFormat::None:   return ~0u;
				case CGVertexFormat::UInt:
				case CGVertexFormat::UInt2:
				case CGVertexFormat::UInt3:
				case CGVertexFormat::UInt4:  return GL_UNSIGNED_INT;
				case CGVertexFormat::Float:
				case CGVertexFormat::Float2:
				case CGVertexFormat::Float3:
				case CGVertexFormat::Float4: return GL_FLOAT;
			}
		
			return ~0u;
		}

		bool CreateVertexArray(const CGBuffer& vBuffer, CGVertexLayout& vLayout)
		{
			uint32_t& vao = vLayout.api.opengl.vao;

			glCreateVertexArrays(1, &vao);

			if (vao == 0u)
			{
				return false;
			}

			const uint32_t vbo = vBuffer.api.opengl.buffer;

			if (vbo == 0u)
			{
				glDeleteVertexArrays(1, &vao);
				return false;
			}

			glVertexArrayVertexBuffer(vao, 0u, vbo, static_cast<GLintptr>(0), static_cast<GLsizei>(vBuffer.desc.stride));

			for (uint8_t i = 0u; i < vLayout.count; ++i)
			{
				const CGVertexElement& element = vLayout.elements[i];
				const CGVertexFormat format = element.format;

				glEnableVertexArrayAttrib(vao, i);
				glVertexArrayAttribFormat(vao, i, GetAttributeCount(format), GetAttributeType(format), GL_FALSE, element.offset);
				glVertexArrayAttribBinding(vao, i, 0);

				if (glGetError() != GL_NO_ERROR)
				{
					glDeleteVertexArrays(1, &vao);
					return false;
				}
			}

			return true;
		}
	}

	namespace RenderOps
	{
		static void Draw(const uint32_t start, const uint32_t count);
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

		static void UseProgram(const uint32_t program)
		{
			glUseProgram(program);
		}

		void ExecuteRenderCommands(const CGRenderContext& context, const CGResourcePool& resourcePool)
		{
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
					case CGRenderCommandType::SetPipelineState:
					{
						UseProgram(cmd.params.setPipelineState.program);

						continue;
					}
					case CGRenderCommandType::SetVertexBuffer:
					{
						const CGBuffer& vertexBuffer = bufferPool.vertexBuffers[cmd.params.setVertexBuffer.buffer];

						BindVertexArray(vertexBuffer.api.opengl.buffer);

						continue;
					}
					case CGRenderCommandType::SetIndexBuffer:
					{
						const CGBuffer& indexBuffer = bufferPool.indexBuffers[cmd.params.setIndexBuffer.buffer];

						continue;
					}
					case CGRenderCommandType::Draw:
					{
						RenderOps::Draw(cmd.params.draw.start, cmd.params.draw.count);
						
						continue;
					}
				}

				break;
			}
		}
	}

	namespace RenderOps
	{
		void Draw(const uint32_t start, const uint32_t count)
		{
			glDrawArrays(GL_TRIANGLES, start, count);
		}
	}

	namespace FrameOps
	{
		void EndFrame(const CGResourcePool& resourcePool)
		{
			//ContextOps::UseProgram(0U);
		}

		void Present(void* window)
		{
			if (!window)
			{
				return;
			}

			glfwSwapBuffers(static_cast<GLFWwindow*>(window));
		}
	}
}
