#include "renderer.h"

#include "platform/window.h";

// renderer.cpp
namespace cg::renderer
{
	bool AddRenderCommands(const uint8_t count, const CGRenderCommand commands[], CGCommandPool& commandPool)
	{
		if (count < 1 || commands == nullptr || commandPool.count + count > CG_MAX_RENDER_COMMANDS)
		{
			return false;
		}

		uint8_t startIndex = commandPool.count;
		for (uint8_t i = 0u; i < count; ++i)
		{
			commandPool.commands[startIndex + i] = commands[i];
		}

		commandPool.count += count;

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
				//ProcessOpenGLRenderCommands(renderer);
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

			if (shaderPool.vsCount + 1u > CG_MAX_VERTEX_SHADERS ||
				shaderPool.fsCount + 1u > CG_MAX_FRAGMENT_SHADERS)
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
					break;
				}
				case CGRendererType::OpenGL:
				{
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
				}
			}

			return true;
		}

		bool CreateVertexLayout(const uint8_t count, CGVertexElement elements[], CGRenderer& renderer, CGShader& vShader, CGVertexLayout& vLayout)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			if (count < 1 || elements == nullptr || bufferPool.vlCount + 1 > CG_MAX_VERTEX_LAYOUTS)
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
					break;
				}
				case CGRendererType::OpenGL:
				{
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
				}
			}

			return true;
		}

		bool CreateVertexBuffer(const CGBufferDesc& vbDesc, CGRenderer& renderer, const void* vbData)
		{
			CGBufferPool& bufferPool = renderer.resourcePool.bufferPool;

			if (bufferPool.vbCount + 1u > CG_MAX_VERTEX_BUFFERS)
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
					CGVertexLayout& vertexLayout = bufferPool.vertexLayouts[bufferPool.vlCount];
					CGBuffer& vertexBuffer = bufferPool.vertexBuffers[bufferPool.vbCount];

					if (!D3D11::DeviceOps::CreateVertexBuffer(renderer.device, vbDesc, vertexBuffer, vbData))
					{
						return false;
					}

					bufferPool.vbCount++;

					break;
				}
				case CGRendererType::Direct3D12:
				{
					break;
				}
				case CGRendererType::OpenGL:
				{
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
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
					break;
				}
				case CGRendererType::OpenGL:
				{
					break;
				}
				case CGRendererType::Vulkan:
				{
					break;
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

		CGRenderCommand SetVertexBuffer(const uint32_t vertexBuffer)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::SetVertexBuffer;
			cmd.params.setVertexBuffer.vertexBuffer = vertexBuffer;

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

		CGRenderCommand Submit(const uint8_t view, const uint32_t program)
		{
			CGRenderCommand cmd = {};

			cmd.type = CGRenderCommandType::Submit;
			cmd.params.submit.program = program;
			cmd.params.submit.view = view;

			return cmd;
		}
	}

	namespace FrameOps
	{
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
