#include "shader.h"

#include "types.h"
#include "io/fileio.h"

// shader.cpp
namespace cg::renderer::ShaderOps
{
	int32_t CreateShader(const CGRendererType rendererType, CGShaderHandlePool& shaderPool, io::CGFile shaderFile, const CGShaderType shaderType)
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
				return CreateOpenGLShader(shaderPool, std::move(shaderFile), shaderType);
			}
			case CGRendererType::Vulkan:
			{
				break;
			}
		}

		return -1;
	}

	int32_t CreateShaderProgram(const CGRendererType rendererType, CGShaderHandlePool& shaderPool, const int32_t shaderCount, const int32_t shaderIndices[])
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
			return CreateOpenGLShaderProgram(shaderPool, shaderCount, shaderIndices);
		}
		case CGRendererType::Vulkan:
		{
			break;
		}
		}

		return -1;
	}

	void DestroyShaderProgram()
	{

	}
}
