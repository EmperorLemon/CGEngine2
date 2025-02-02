#pragma once

#include <cstdint>

namespace cg 
{
	namespace io
	{
		struct CGFile;
	}

	enum class CGRendererType : uint8_t;
}

// shader.h
namespace cg::renderer
{
	constexpr uint16_t CG_MAX_SHADERS = 512;
	constexpr uint8_t CG_MAX_SHADER_PROGRAMS = 64;

	enum class CGShaderType : uint8_t
	{
		None = 0,
		Vertex = 1,
		Fragment = 2,
		Geometry = 3,
		TessellationControl = 4,
		TessellationEvaluation = 5,
		Compute = 6,
		Program = 7,
	};

	struct CGShaderHandlePool
	{
		uint32_t shaders[CG_MAX_SHADERS] = {};
		CGShaderType shaderTypes[CG_MAX_SHADERS] = {};
		uint32_t shaderPrograms[CG_MAX_SHADER_PROGRAMS] = {};
		int16_t shaderCount = 0;
		int16_t shaderProgramCount = 0;
	};

	namespace ShaderOps
	{
		int32_t CreateShader(CGRendererType rendererType, CGShaderHandlePool& shaderPool, io::CGFile shaderFile, CGShaderType shaderType);
		int32_t CreateShaderProgram(CGRendererType rendererType, CGShaderHandlePool& shaderPool, int32_t shaderCount, const int32_t shaderIndices[]);
		int32_t CreateOpenGLShader(CGShaderHandlePool& shaderPool, io::CGFile&& shaderFile, CGShaderType shaderType);
		int32_t CreateOpenGLShaderProgram(CGShaderHandlePool& shaderPool, int32_t shaderCount, const int32_t shaderIndices[]);

		void DestroyShaderProgram();
		void DestroyOpenGLShaderProgram();
	}
}
