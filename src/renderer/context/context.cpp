#include "context.h"

#include "platform/window.h"

// context.cpp
namespace cg::renderer
{
	namespace FrameOps
	{
		void Present(const CGRenderContext& rendercontext, const core::CGWindow& window)
		{
			switch (rendercontext.rendererType)
			{
				case CGRendererType::None:
				{
					break;
				}
				case CGRendererType::Direct3D11:
				{
					rendercontext.apiFunctions.present(rendercontext.d3d11Context.swapchain);
					break;
				}
				case CGRendererType::Direct3D12:
				{
					break;
				}
				case CGRendererType::OpenGL:
				{
					rendercontext.apiFunctions.present(window.winptr);
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
