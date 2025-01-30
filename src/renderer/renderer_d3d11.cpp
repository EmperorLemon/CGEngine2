#include "renderer.h"

#include "context_d3d11.h"

// renderer_d3d11.cpp
namespace cg::renderer
{
	void SetupD3D11RenderFunctions(CGRenderer& renderer, const CGRenderContext& context)
	{
		auto data = context.GetData<CGRendererType::Direct3D11>();

		CGRenderAPIFunctions renderAPIFunctions;

		renderAPIFunctions.ClearColor =
			[deviceContext = data->deviceContext, renderTargetView = data->renderTargetView]
			(const uint32_t flags, const float r, const float g, const float b, const float a)
		{
			if (deviceContext && renderTargetView)
			{
				float color[4] = { r, g, b, a };
				deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
			}
		};

		renderer.SetRenderAPIFunctions(renderAPIFunctions);
	}
}