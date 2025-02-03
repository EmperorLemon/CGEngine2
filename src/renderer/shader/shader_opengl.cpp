#include <glad/gl.h>
#include <cstdio>

#include "shader.h"
#include "io/fileio.h"

// shader_opengl.cpp
namespace cg::renderer::ShaderOps
{
	static constexpr GLenum GetShaderType(const CGShaderType shaderType)
	{
		switch (shaderType)
		{
		case CGShaderType::None:
			return ~0u;
		case CGShaderType::Vertex:
			return GL_VERTEX_SHADER;
		case CGShaderType::Fragment:
			return GL_FRAGMENT_SHADER;
		case CGShaderType::Geometry:
			return GL_GEOMETRY_SHADER;
		case CGShaderType::TessellationControl:
			return GL_TESS_CONTROL_SHADER;
		case CGShaderType::TessellationEvaluation:
			return GL_TESS_EVALUATION_SHADER;
		case CGShaderType::Compute:
			return GL_COMPUTE_SHADER;
		case CGShaderType::Program:
			return ~0u;
		}

		return ~0u;
	}

	constexpr static void CheckShaderCompileErrors(const GLuint shader, const CGShaderType shaderType)
	{
		GLint  success = 0;
		GLchar infoLog[1024] = {};

		const auto shaderTypeStr = [shaderType]
		{
			switch (shaderType)
			{
			case CGShaderType::None:
				break;
			case CGShaderType::Vertex:
				return "Vertex shader";
			case CGShaderType::Fragment:
				return "Fragment shader";
			case CGShaderType::Geometry:
				return "Geometry shader";
			case CGShaderType::TessellationControl:
				return "Tessellation control shader";
			case CGShaderType::TessellationEvaluation:
				return "Tessellation evaluation shader";
			case CGShaderType::Compute:
				return "Computer shader";
			case CGShaderType::Program:
				return "Shader program";
			}

			return "Unknown shader";
		}();

		if (shaderType != CGShaderType::Program)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				printf("Error::SHADER_COMPILATION_ERROR of type: %s\n%s", shaderTypeStr, infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				printf("Error::PROGRAM_LINKING_ERROR of type: %s\n%s", shaderTypeStr, infoLog);
			}
		}
	}

	int32_t CreateOpenGLShader(CGShaderHandlePool& shaderPool, io::CGFile&& shaderFile, const CGShaderType shaderType)
	{
		if (shaderPool.shaderCount >= CG_MAX_SHADERS || shaderFile.data == nullptr || shaderType == CGShaderType::None)
		{
			return -1;
		}

		int32_t index = shaderPool.shaderCount;
		uint32_t& shader = shaderPool.shaders[index];

		char* shaderSource = shaderFile.data.get();

		shader = glCreateShader(GetShaderType(shaderType));
		glShaderSource(shader, 1, &shaderSource, nullptr);
		glCompileShader(shader);
		CheckShaderCompileErrors(shader, shaderType);

		if (glGetError() != GL_NO_ERROR)
		{
			glDeleteShader(shader);
			return -1;
		}

		shaderPool.shaderCount++;

		return index;
	}

	int32_t CreateOpenGLShaderProgram(CGShaderHandlePool& shaderPool, const int32_t shaderCount, const int32_t shaderIndices[])
	{
		if (shaderPool.shaderProgramCount >= CG_MAX_SHADER_PROGRAMS || shaderPool.shaderCount < shaderCount
			|| shaderCount < 2 || shaderIndices == nullptr)
		{
			return -1;
		}

		int32_t index = shaderPool.shaderProgramCount;
		uint32_t& program = shaderPool.shaderPrograms[index];

		program = glCreateProgram();
		for (int32_t i = 0; i < shaderCount; ++i)
		{
			const uint32_t shader = shaderPool.shaders[shaderIndices[i]];

			glAttachShader(program, shader);
		}
		glLinkProgram(program);
		CheckShaderCompileErrors(program, CGShaderType::Program);

		if (glGetError() != GL_NO_ERROR)
		{
			for (int32_t i = 0; i < shaderCount; ++i)
			{
				const uint32_t shader = shaderPool.shaders[shaderIndices[i]];

				glDeleteShader(shader);
			}

			glDeleteProgram(program);
		}

		for (int32_t i = 0; i < shaderCount; ++i)
		{
			const uint32_t shader = shaderPool.shaders[shaderIndices[i]];

			glDeleteShader(shader);
		}

		shaderPool.shaderProgramCount++;

		return index;
	}

	void DestroyOpenGLShaderProgram()
	{

	}
}
