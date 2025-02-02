#pragma once

#include "layout.h"

namespace cg { enum class CGRendererType : uint8_t; }

// buffer.h
namespace cg::renderer
{
	constexpr uint16_t CG_MAX_VERTEX_BUFFERS = 512;
	constexpr uint16_t CG_MAX_INDEX_BUFFERS = 512;
	constexpr uint16_t CG_MAX_VERTEX_ARRAYS = 512;

	// Vertex-Index Buffer Pool
	struct CGVIBufferPool
	{
		// Data block
		uint32_t vertexBuffers[CG_MAX_VERTEX_BUFFERS] = {};
		uint32_t indexBuffers[CG_MAX_INDEX_BUFFERS] = {};
		uint32_t vertexArrays[CG_MAX_VERTEX_ARRAYS] = {};

		// Sizes block
		int32_t vertexSizes[CG_MAX_VERTEX_BUFFERS] = {};
		int32_t indexSizes[CG_MAX_INDEX_BUFFERS] = {};

		// Counts block
		int32_t vertexBufferCount = 0;
		int32_t indexBufferCount = 0;
		int32_t vertexArrayCount = 0;
	};

	namespace BufferOps
	{
		/* For OpenGL this will return the index of the vertex array object */
		int32_t CreateVertexBuffer(CGRendererType rendererType, CGVIBufferPool& bufferPool, int32_t vertexSize, const void* vertexData, CGVertexLayout& vertexLayout);
		int32_t CreateIndexBuffer(CGRendererType rendererType, CGVIBufferPool& bufferPool, int32_t indexSize, const void* indexData);


		int32_t CreateOpenGLVertexBuffer(CGVIBufferPool& viBufferPool, int32_t vertexSize, const void* vertexData);
		int32_t CreateOpenGLIndexBuffer(CGVIBufferPool& viBufferPool, int32_t indexSize, const void* indexData);
		int32_t CreateOpenGLVertexArray(CGVIBufferPool& viBufferPool, int32_t vertexBufferIdx, CGVertexLayout& vertexLayout);
		int32_t AttachOpenGLIndexBuffer(CGVIBufferPool& viBufferPool, int32_t indexBufferIdx);

		void DestroyVertexBuffers(CGRendererType rendererType, CGVIBufferPool& bufferPool);
		void DestroyIndexBuffers(CGRendererType rendererType, CGVIBufferPool& bufferPool);
	}
}
