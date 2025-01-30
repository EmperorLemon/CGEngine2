#pragma once

#include <cstdint>

// types.h
namespace cg
{
	enum class CGRendererType : uint32_t
	{
		None = 0,
		Direct3D11,
		Direct3D12,
		OpenGL,
		Vulkan
	};
}