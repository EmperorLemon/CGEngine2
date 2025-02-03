#include <d3d11.h>
#include <dxgi.h>

#include "renderer.h"

// renderer_d3d11.cpp
namespace cg::renderer::RenderOps::D3D11
{
	void D3D11ClearView(
		void* deviceCtx, void* renderTV,
		CGClearFlags clearFlags,
		const float r, const float g, const float b, const float a)
	{
		if (!deviceCtx || !renderTV)
		{
			return;
		}

		const auto deviceContext = static_cast<ID3D11DeviceContext*>(deviceCtx);
		const auto renderTargetView = static_cast<ID3D11RenderTargetView*>(renderTV);

		const float rgba[4] = { r, g, b, a };

		deviceContext->ClearRenderTargetView(renderTargetView, rgba);
	}
}
