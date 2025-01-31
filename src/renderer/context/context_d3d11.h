#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "context.hpp"

using Microsoft::WRL::ComPtr;

// context_d3d11.h
namespace cg::renderer
{
	template<>
	struct CGRenderContextData<CGRendererType::Direct3D11> : CGRenderContextDataBase
	{
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> deviceContext;
		ComPtr<IDXGISwapChain> swapchain;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
	};
}