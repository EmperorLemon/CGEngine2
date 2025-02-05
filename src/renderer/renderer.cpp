#include "renderer.h"

// renderer.cpp
namespace cg::renderer
{
	bool AddRenderCommands(CGRenderer& renderer, int8_t count, const CGRenderCommand commands[])
	{
		if (count <= 0 || commands == nullptr || renderer.commandList.count + count > CG_MAX_RENDER_COMMANDS)
		{
			return false;
		}

		int8_t startIndex = renderer.commandList.count;
		for (int8_t i = 0; i < count; ++i)
		{
			renderer.commandList.commands[startIndex + i] = commands[i];
		}

		renderer.commandList.count += count;

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
				D3D11::RenderOps::ExecuteRenderCommands(renderer.context, renderer.commandList.count, renderer.commandList.commands);
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

	namespace RenderOps
	{
		CGRenderCommand SetViewClear(const int8_t view, const CGClearFlags flags, const uint32_t color)
		{
			CGRenderCommand command = {};

			command.commandType = CGRenderCommandType::SetViewClear;
			command.params.setViewClear.color = color;
			command.params.setViewClear.clearFlags = flags;
			command.params.setViewClear.view = view;

			return command;
		}

		CGRenderCommand SetVertexBuffer(const uint32_t vertexBuffer)
		{
			CGRenderCommand command = {};

			command.commandType = CGRenderCommandType::SetVertexBuffer;
			command.params.setVertexBuffer.vertexBuffer = vertexBuffer;

			return command;
		}

		CGRenderCommand DrawIndexed(const int32_t count)
		{
			CGRenderCommand command = {};

			command.commandType = CGRenderCommandType::SetIndexBuffer;
			command.params.drawIndexed.count = count;

			return command;
		}

		CGRenderCommand Submit(const int8_t view, const uint32_t program)
		{
			CGRenderCommand command = {};

			command.commandType = CGRenderCommandType::Submit;
			command.params.submit.program = program;
			command.params.submit.view = view;

			return command;
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
