#include "renderer.h"

// renderer.cpp
namespace cg::renderer
{
	constexpr float ONE_OVER_255 = 0.003921568627451f;

	static void ProcessOpenGLRenderCommands(const CGRenderer& renderer);
	static void ProcessD3D11RenderCommands(const CGRenderer& renderer);

	CGRenderer::CGRenderer(CGRenderContext& ctx) : renderContext(ctx)
	{

	}  

	bool AddRenderCommands(CGRenderer& renderer, int16_t count, const CGRenderCommand commands[])
	{
		if (count <= 0 || commands == nullptr || renderer.renderCommandCount + count > CG_MAX_RENDER_COMMANDS)
		{
			return false;
		}

		int16_t startIndex = renderer.renderCommandCount;
		for (int16_t i = 0; i < count; ++i)
		{
			renderer.renderCommands[startIndex + i] = commands[i];
		}

		renderer.renderCommandCount += count;

		return true;
	}

	void SubmitRenderCommands(const CGRenderer& renderer)
	{
		switch (renderer.renderContext.rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				ProcessD3D11RenderCommands(renderer);
				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				ProcessOpenGLRenderCommands(renderer);
				break;
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}
	}

	void ProcessOpenGLRenderCommands(const CGRenderer& renderer)
	{
		for (uint16_t i = 0; i < renderer.renderCommandCount; ++i)
		{
			const CGRenderCommand& command = renderer.renderCommands[i];

			switch (command.commandType)
			{
				case CG_NONE:
				{
					break;
				}
				case CG_CLEAR:
				{
					uint32_t color = command.params.clear.color;

					RenderOps::OpenGL::OpenGLClearView(
						command.params.clear.clearFlags,
						((color >> 24) & 0xFF) * ONE_OVER_255,
						((color >> 16) & 0xFF) * ONE_OVER_255,
						((color >> 8) & 0xFF) * ONE_OVER_255,
						(color & 0xFF) * ONE_OVER_255
					);

					continue;
				}
			}

			break;
		}
	}

	void ProcessD3D11RenderCommands(const CGRenderer& renderer)
	{
		for (uint16_t i = 0; i < renderer.renderCommandCount; ++i)
		{
			const CGRenderCommand& command = renderer.renderCommands[i];

			switch (command.commandType)
			{
				case CG_NONE:
				{
					break;
				}
				case CG_CLEAR:
				{
					uint32_t color = command.params.clear.color;

					RenderOps::D3D11::D3D11ClearView(
						renderer.renderContext.d3d11Context.deviceContext,
						renderer.renderContext.d3d11Context.renderTargets[command.params.clear.view],
						command.params.clear.clearFlags,
						((color >> 24) & 0xFF) * ONE_OVER_255,
						((color >> 16) & 0xFF) * ONE_OVER_255,
						((color >> 8) & 0xFF) * ONE_OVER_255,
						(color & 0xFF) * ONE_OVER_255
					);

					continue;
				}
			}

			break;
		}
	}

	namespace RenderOps
	{
		CGRenderCommand ClearView(const int16_t view, const CGClearFlags flags, const uint32_t color)
		{
			CGRenderCommand command = {};

			command.commandType = CG_CLEAR;
			command.params.clear.view = view;
			command.params.clear.clearFlags = flags;
			command.params.clear.color = color;

			return command;
		}
	}
}
