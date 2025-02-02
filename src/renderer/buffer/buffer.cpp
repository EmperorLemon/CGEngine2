#include "buffer.h"

#include "types.h"

namespace cg::renderer::BufferOps
{
	int32_t CreateVertexBuffer(const CGRendererType rendererType, CGVIBufferPool& bufferPool, const int32_t vertexSize, const void* vertexData, CGVertexLayout& vertexLayout)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				const int32_t vertexBufferIdx = CreateOpenGLVertexBuffer(bufferPool, vertexSize, vertexData);
				
				return CreateOpenGLVertexArray(bufferPool, vertexBufferIdx, vertexLayout);
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return -1;
	}

	int32_t CreateIndexBuffer(const CGRendererType rendererType, CGVIBufferPool& bufferPool, const int32_t indexSize, const void* indexData)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
				break;
			}
			case CGRendererType::Direct3D12:
			{
				break;
			}
			case CGRendererType::OpenGL:
			{
				const int32_t indexBufferIdx = CreateOpenGLIndexBuffer(bufferPool, indexSize, indexData);

				AttachOpenGLIndexBuffer(bufferPool, indexBufferIdx);

				return indexBufferIdx;
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return -1;
	}

	void DestroyVertexBuffers(const CGRendererType rendererType, CGVIBufferPool& bufferPool)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
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

	void DestroyIndexBuffers(const CGRendererType rendererType, CGVIBufferPool& bufferPool)
	{
		switch (rendererType)
		{
			case CGRendererType::None:
			{
				break;
			}
			case CGRendererType::Direct3D11:
			{
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