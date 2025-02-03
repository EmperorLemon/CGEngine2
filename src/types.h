#pragma once

#include <cstdint>

// types.h
namespace cg
{
	enum class CGRendererType : uint8_t
	{
		None = 0,
		Direct3D11 = 1,
		Direct3D12 = 2,
		OpenGL = 3,
		Vulkan = 4,
	};
}
