#pragma once

#include <cstdint>

// layout.h
namespace cg::renderer
{
	constexpr uint8_t CG_MAX_VERTEX_ELEMENTS = 16;

	enum class CGVertexAttribute : uint8_t 
	{
		None,
		Position,
		Normal,
		TexCoord0,
	};

	enum class CGDataType : uint8_t
	{
		None,
		UInt8,
		UInt16,
		UInt32,
		Float,
		Float2,
		Float3,
		Float4
	};

	struct CGVertexElement
	{
		CGVertexAttribute attribute = CGVertexAttribute::None;
		CGDataType dataType = CGDataType::None;
		uint32_t offset = 0u;
	};

	struct CGVertexLayout
	{
		CGVertexElement vertexElements[CG_MAX_VERTEX_ELEMENTS] = {};
		uint8_t elementCount = 0;
	};

	constexpr int32_t GetDataTypeSize(const CGDataType dataType)
	{
		switch (dataType)
		{
			case CGDataType::None:	  return -1;
			case CGDataType::UInt8:   return sizeof(uint8_t);
			case CGDataType::UInt16:  return sizeof(uint16_t);
			case CGDataType::UInt32:  return sizeof(uint32_t);
			case CGDataType::Float:   return sizeof(float);
			case CGDataType::Float2:  return 2 * sizeof(float);
			case CGDataType::Float3:  return 3 * sizeof(float);
			case CGDataType::Float4:  return 4 * sizeof(float);
		}

		return -1;
	}

	constexpr int32_t GetByteStride(const CGVertexLayout& layout)
	{
		int32_t byteStride = 0;

		for (uint8_t i = 0; i < layout.elementCount; ++i)
		{
			const int32_t dataSize = GetDataTypeSize(layout.vertexElements[i].dataType);
			byteStride += dataSize;
		}

		return byteStride;
	}

	constexpr int32_t GetAttributeCount(const CGDataType dataType)
	{
		switch (dataType)
		{
			case CGDataType::None:   return -1;
			case CGDataType::UInt8:  return 1;
			case CGDataType::UInt16: return 1;
			case CGDataType::UInt32: return 1;
			case CGDataType::Float:  return 1;
			case CGDataType::Float2: return 2;
			case CGDataType::Float3: return 3;
			case CGDataType::Float4: return 4;
			default: break;
		}

		return -1;
	}

	constexpr void GetAttributeOffset(CGVertexLayout& layout)
	{
		uint32_t offset = 0u;

		for (uint8_t i = 0; i < layout.elementCount; ++i)
		{
			const int32_t size = GetDataTypeSize(layout.vertexElements[i].dataType);
			
			if (i > 0)
			{
				offset += static_cast<uint32_t>(size);
				layout.vertexElements[i].offset = offset;
			}
		}
	}
}
