#include "buffer.h"

#include <glad/gl.h>

// buffer_opengl.cpp
namespace cg::renderer::BufferOps
{
	static constexpr GLenum GetAttributeType(const CGDataType dataType)
	{
		switch (dataType)
		{
			case CGDataType::None:
				break;
			case CGDataType::UInt8:
				return GL_UNSIGNED_BYTE;
			case CGDataType::UInt16:
				return GL_UNSIGNED_SHORT;
			case CGDataType::UInt32:
				return GL_UNSIGNED_INT;
			case CGDataType::Float:
			case CGDataType::Float2:
			case CGDataType::Float3:
			case CGDataType::Float4:
				return GL_FLOAT;
		}

		return ~0u;
	}

	int32_t CreateOpenGLVertexBuffer(CGVIBufferPool& viBufferPool, const int32_t vertexSize, const void* vertexData)
	{
		if (viBufferPool.vertexBufferCount >= CG_MAX_VERTEX_BUFFERS || vertexSize <= 0 || vertexData == nullptr)
		{
			return -1;
		}

		int32_t index = viBufferPool.vertexBufferCount;
		uint32_t& buffer = viBufferPool.vertexBuffers[index];

		glCreateBuffers(1, &buffer);

		if (buffer == 0)
		{
			return -1;
		}

		glNamedBufferStorage(buffer, static_cast<GLsizeiptr>(vertexSize), vertexData, 0);

		// Check for OpenGL errors after buffer creation
		if (glGetError() != GL_NO_ERROR)
		{
			glDeleteBuffers(1, &buffer);
			return -1;
		}

		viBufferPool.vertexSizes[index] = vertexSize;
		viBufferPool.vertexBufferCount++;

		return index;
	}

	int32_t CreateOpenGLIndexBuffer(CGVIBufferPool& viBufferPool, const int32_t indexSize, const void* indexData)
	{
		if (viBufferPool.indexBufferCount >= CG_MAX_INDEX_BUFFERS || indexSize <= 0 || indexData == nullptr)
		{
			return -1;
		}

		int32_t index = viBufferPool.indexBufferCount;
		uint32_t& buffer = viBufferPool.indexBuffers[index];

		glCreateBuffers(1, &buffer);

		if (buffer == 0)
		{
			return -1;
		}

		glNamedBufferStorage(buffer, static_cast<GLsizeiptr>(indexSize), indexData, 0);

		// Check for OpenGL errors after buffer creation
		if (glGetError() != GL_NO_ERROR)
		{
			glDeleteBuffers(1, &buffer);
			return -1;
		}

		viBufferPool.indexSizes[index] = indexSize;
		viBufferPool.indexBufferCount++;

		return index;
	}

	int32_t CreateOpenGLVertexArray(CGVIBufferPool& viBufferPool, const int32_t vertexBufferIdx, CGVertexLayout& vertexLayout)
	{
		if (viBufferPool.vertexArrayCount >= CG_MAX_VERTEX_ARRAYS || vertexBufferIdx < 0)
		{
			return -1;
		}

		int32_t index = viBufferPool.vertexArrayCount;
		uint32_t& vertexArray = viBufferPool.vertexArrays[index];

		glCreateVertexArrays(1, &vertexArray);

		if (vertexArray == 0)
		{
			return -1;
		}

		const uint32_t vbo = viBufferPool.vertexBuffers[vertexBufferIdx];

		if (vbo == 0)
		{
			glDeleteVertexArrays(1, &vertexArray);
			return -1;
		}

		glVertexArrayVertexBuffer(vertexArray, 0, vbo, 0, GetByteStride(vertexLayout));

		GetAttributeOffset(vertexLayout);

		for (uint32_t i = 0; i < vertexLayout.elementCount; ++i)
		{
			const CGVertexElement& vertexElement = vertexLayout.vertexElements[i];
			const CGDataType dataType = vertexLayout.vertexElements[i].dataType;

			glEnableVertexArrayAttrib(vertexArray, i);
			glVertexArrayAttribFormat(vertexArray, i, GetAttributeCount(dataType), GetAttributeType(dataType), GL_FALSE, vertexElement.offset);
			glVertexArrayAttribBinding(vertexArray, i, 0);

			if (glGetError() != GL_NO_ERROR)
			{
				glDeleteVertexArrays(1, &vertexArray);
				return -1;
			}
		}
		
		viBufferPool.vertexArrayCount++;

		return index;
	}

	int32_t AttachOpenGLIndexBuffer(CGVIBufferPool& viBufferPool, const int32_t indexBufferIdx)
	{
		if (indexBufferIdx < 0 || indexBufferIdx >= viBufferPool.indexBufferCount)
		{
			return -1;
		}

		const int32_t vaoIndex = viBufferPool.vertexArrayCount - 1;
		if (vaoIndex < 0)
		{
			return -1;
		}

		const uint32_t vao = viBufferPool.vertexArrays[vaoIndex];
		const uint32_t ibo = viBufferPool.indexBuffers[indexBufferIdx];

		glVertexArrayElementBuffer(vao, ibo);

		if (glGetError() != GL_NO_ERROR)
		{
			return -1;
		}

		return vaoIndex;
	}
}
