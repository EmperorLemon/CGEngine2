#include "renderer.h"
#include "platform/window.h"

// renderer.cpp
namespace cg::renderer
{
	bool AddRenderCommands(const uint8_t count, const CGRenderCommand commands[], CGRenderer& renderer)
	{
		CGCommandPool& cmdPool = renderer.resourcePool.commandPool;

		if (count < 1 || commands == nullptr || cmdPool.count + count >= CG_MAX_RENDER_COMMANDS)
		{
			return false;
		}

		uint8_t startIndex = cmdPool.count;
		for (uint8_t i = 0u; i < count; ++i)
		{
			cmdPool.commands[startIndex + i] = commands[i];
		}

		cmdPool.count += count;

		return true;
	}

	void ExecuteRenderCommands(const CGRenderer& renderer)
	{
		switch (renderer.type)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				D3D11::ContextOps::ExecuteRenderCommands(renderer.context, renderer.resourcePool);
				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				OpenGL::ContextOps::ExecuteRenderCommands(renderer.context, renderer.resourcePool);
				break;
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}
	}

	namespace DeviceOps
	{
		constexpr static uint32_t GetAttributeSize(const CGVertexFormat format)
		{
			switch (format)
			{
				case CGVertexFormat::None:	 break;
				case CGVertexFormat::UInt:	 return sizeof(uint32_t);
				case CGVertexFormat::UInt2:	 return sizeof(uint32_t) * 2u;
				case CGVertexFormat::UInt3:	 return sizeof(uint32_t) * 3u;
				case CGVertexFormat::UInt4:	 return sizeof(uint32_t) * 4u;
				case CGVertexFormat::Float:  return sizeof(float);
				case CGVertexFormat::Float2: return sizeof(float) * 2u;
				case CGVertexFormat::Float3: return sizeof(float) * 3u;
				case CGVertexFormat::Float4: return sizeof(float) * 4u;
			}

			return 0u;
		};

		bool CreateShader(const CGShaderDesc& desc, CGRenderer& renderer, CGShader& shader)
		{
			CGShaderPool& shaderPool = renderer.resourcePool.shaderPool;

			if (desc.shaderType == CGShaderType::None || 
				shaderPool.vsCount + 1u >= CG_MAX_VERTEX_SHADERS || shaderPool.fsCount + 1u >= CG_MAX_FRAGMENT_SHADERS)
			{
				return false;
			}

			shader.type = desc.shaderType;

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					if (!D3D11::DeviceOps::CreateShader(renderer.context, desc, shader))
					{
						return false;
					}

					switch (desc.shaderType)
					{
						case CGShaderType::None:
						{
							return false;
						}
						case CGShaderType::Vertex:
						{
							shaderPool.vertexShaders[shaderPool.vsCount] = shader;
							shaderPool.vsCount++;
							break;
						}
						case CGShaderType::Fragment:
						{
							shaderPool.fragmentShaders[shaderPool.fsCount] = shader;
							shaderPool.fsCount++;
							break;
						}
					}

					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					if (!OpenGL::DeviceOps::CreateShader(desc, shader))
					{
						return false;
					}

					switch (desc.shaderType)
					{
						case CGShaderType::Vertex:
						{
							shaderPool.vertexShaders[shaderPool.vsCount] = shader;
							shaderPool.vsCount++;
							break;
						}
						case CGShaderType::Fragment:
						{
							shaderPool.fragmentShaders[shaderPool.fsCount] = shader;
							shaderPool.fsCount++;
							break;
						}
					}

					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}

		bool CreateShaderProgram(const uint8_t count, const CGShader shaders[], CGRenderer& renderer, uint32_t& program)
		{
			CGShaderPool& shaderPool = renderer.resourcePool.shaderPool;

			if (count < 1 || shaders == nullptr || shaderPool.pCount + count >= CG_MAX_SHADER_PROGRAMS)
			{
				return false;
			}

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					if (!OpenGL::DeviceOps::CreateShaderProgram(count, shaders, program))
					{
						return false;
					}

					shaderPool.programs[shaderPool.pCount] = program;
					shaderPool.pCount++;

					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}

		bool SetupVertexLayout(const uint8_t count, CGVertexElement elements[], CGRenderer& renderer, CGVertexLayout& vLayout)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			if (count < 1 || elements == nullptr || bufferPool.vlCount + 1 >= CG_MAX_VERTEX_LAYOUTS)
			{
				return false;
			}

			uint32_t offset = 0u;
			uint32_t size = 0u;

			for (uint8_t i = 0; i < count; ++i)
			{
				elements[i].size = GetAttributeSize(elements[i].format);
				size += elements[i].size;

				if (i > 0)
				{
					offset += elements[i - 1].size;
					elements[i].offset = offset;
				}

				vLayout.elements[i] = elements[i];
			}

			vLayout.count = count;
			vLayout.size = size;

			return true;
		}

		bool CreateVertexLayout(const CGBuffer& vBuffer, CGRenderer& renderer, CGShader& vShader, CGVertexLayout& vLayout)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					if (!D3D11::DeviceOps::CreateVertexLayout(renderer.device, vShader, vLayout))
					{
						return false;
					}

					bufferPool.vertexLayouts[bufferPool.vlCount] = vLayout;
					bufferPool.vlCount++;

					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					if (!OpenGL::DeviceOps::CreateVertexArray(vBuffer, vLayout))
					{
						return false;
					}

					bufferPool.vertexLayouts[bufferPool.vlCount] = vLayout;
					bufferPool.vlCount++;

					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}

		bool CreateVertexBuffer(const CGBufferDesc& vbDesc, CGRenderer& renderer, CGBuffer& vBuffer, const void* vbData)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			if (bufferPool.vbCount + 1u > CG_MAX_VERTEX_BUFFERS)
			{
				return false;
			}

			vBuffer.desc = vbDesc;

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					if (!D3D11::DeviceOps::CreateVertexBuffer(renderer.device, vbDesc, vBuffer, vbData))
					{
						return false;
					}

					bufferPool.vertexBuffers[bufferPool.vbCount] = vBuffer;
					bufferPool.vbCount++;

					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					if (!OpenGL::DeviceOps::CreateVertexBuffer(vbDesc, vBuffer, vbData))
					{
						return false;
					}

					bufferPool.vertexBuffers[bufferPool.vbCount] = vBuffer;
					bufferPool.vbCount++;

					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}

		bool CreateIndexBuffer(const CGBufferDesc& ibDesc, CGRenderer& renderer, CGBuffer& iBuffer, const void* ibData)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			if (bufferPool.ibCount + 1u > CG_MAX_INDEX_BUFFERS)
			{
				return false;
			}

			iBuffer.desc = ibDesc;

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					//if (!D3D11::DeviceOps::CreateIndexBuffer(renderer.device, ibDesc, iBuffer, ibData))
					//{
					//	return false;
					//}

					//bufferPool.indexBuffers[bufferPool.ibCount] = iBuffer;
					//bufferPool.ibCount++;

					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					if (!OpenGL::DeviceOps::CreateIndexBuffer(ibDesc, iBuffer, ibData))
					{
						return false;
					}

					bufferPool.indexBuffers[bufferPool.ibCount] = iBuffer;
					bufferPool.ibCount++;

					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}
	}

	namespace ContextOps
	{
		bool CreateViewport(const core::CGWindow& window, CGRenderer& renderer)
		{
			CGRenderContext& context = renderer.context;

			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					return false;
				}
				case CGRendererType::Direct3D11:
				{
					CGViewport& viewport = context.api.d3d11.viewports[context.api.d3d11.viewportCount];

					if (!D3D11::ContextOps::CreateViewport(window.width, window.height, viewport))
					{
						return false;
					}

					context.api.d3d11.viewportCount++;

					break;
				}
				case CGRendererType::Direct3D12:
				{
					return false;
				}
				case CGRendererType::OpenGL:
				{
					break;
				}
				case CGRendererType::Vulkan:
				{
					return false;
				}
			}

			return true;
		}

		CGRenderCommand SetViewClear(const uint8_t view, const CGClearFlags flags, const uint32_t color)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetViewClear;
			cmd.params.setViewClear.color = color;
			cmd.params.setViewClear.clearFlags = flags;
			cmd.params.setViewClear.view = view;

			return cmd;
		}

		CGRenderCommand SetViewport(const uint8_t viewport)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetViewport;
			cmd.params.setViewport.viewport = viewport;

			return cmd;
		}

		CGRenderCommand SetPipelineState(const uint32_t program)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetPipelineState;
			cmd.params.setPipelineState.program = program;

			return cmd;
		}

		CGRenderCommand SetVertexBuffer(const uint32_t vertexBuffer)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetVertexBuffer;
			cmd.params.setVertexBuffer.buffer = vertexBuffer;

			return cmd;
		}

		CGRenderCommand SetIndexBuffer(const uint32_t indexBuffer)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetIndexBuffer;
			cmd.params.setIndexBuffer.buffer = indexBuffer;

			return cmd;
		}

		CGRenderCommand SetVertexShader(const uint8_t shader)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetVertexShader;
			cmd.params.setShader.shader = shader;

			return cmd;
		}

		CGRenderCommand SetFragmentShader(const uint8_t shader)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetFragmentShader;
			cmd.params.setShader.shader = shader;

			return cmd;
		}
	}

	namespace RenderOps
	{
		CGRenderCommand Draw(const uint8_t vertexBuffer, const uint32_t count, const uint32_t start)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::Draw;
			cmd.params.draw.vertexBuffer = vertexBuffer;
			cmd.params.draw.count = count;
			cmd.params.draw.start = start;

			return cmd;
		}

		CGRenderCommand DrawIndexed(const uint32_t count)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::DrawIndexed;
			cmd.params.drawIndexed.count = count;

			return cmd;
		}
	}

	namespace FrameOps
	{
		void EndFrame(const CGRenderer& renderer)
		{
			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					break;
				}
				case CGRendererType::Direct3D11:
				{
					break;
				}
				case CGRendererType::Direct3D12:
				{
					break;
				}
				case CGRendererType::OpenGL:
				{
					OpenGL::FrameOps::EndFrame(renderer.resourcePool);
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
				}
			}
		}

		void Present(const CGRenderer& renderer)
		{
			switch (renderer.type)
			{
				case CGRendererType::None:
				{
					break;
				}
				case CGRendererType::Direct3D11:
				{
					D3D11::FrameOps::Present(renderer.context.api.d3d11.swapchain);
					break;
				}
				case CGRendererType::Direct3D12:
				{
					break;
				}
				case CGRendererType::OpenGL:
				{
					OpenGL::FrameOps::Present(renderer.context.api.opengl.window);
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
				}
			}
		}
	}
}
