#include <cstdio>

#include "cgengine.h"
#include "io/fileio.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

//constexpr float vertices[9] =
//{
//	-0.5f, -0.5f, 0.0f,
//	 0.5f, -0.5f, 0.0f,
//	 0.0f,  0.5f, 0.0f
//};

constexpr float vertices[12] = { 
   0.5f,  0.5f, 0.0f,							  
	 0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f
};

constexpr uint16_t indices[6] = 
{
	0, 1, 3,
	1, 2, 3
};

using namespace cg;
using namespace cg::renderer;

// main.cpp
int main()
{
	CGEngineCreateInfo info;
	info.rendererType = CGRendererType::OpenGL;
	info.resolution.width = WINDOW_WIDTH;
	info.resolution.height = WINDOW_HEIGHT;
	info.debug = true;

	CGEngine engine(info);

	const auto& renderer = engine.GetRenderer();
	auto& context = engine.GetRenderContext();

	CGVertexLayout vertexLayout = {};
	vertexLayout.vertexElements[0] = { CGVertexAttribute::Position, renderer::CGDataType::Float3 };
	vertexLayout.elementCount = 1;

	const int32_t vertexBufferIndex = BufferOps::CreateVertexBuffer(info.rendererType, context.bufferPool, sizeof(vertices), vertices, vertexLayout);
	const int32_t indexBufferIndex = BufferOps::CreateIndexBuffer(info.rendererType, context.bufferPool, sizeof(indices), indices);
	BufferOps::AttachOpenGLIndexBuffer(context.bufferPool, indexBufferIndex);

	printf("\n\n");
	printf("Vertex buffer index: %d\n", vertexBufferIndex);
	printf("Index buffer index: %d\n\n", indexBufferIndex);

	const int32_t shaderIndices[2] = { ShaderOps::CreateShader(info.rendererType, context.shaderPool, io::ReadFile("assets/debug_vs.glsl"), CGShaderType::Vertex),
									   ShaderOps::CreateShader(info.rendererType, context.shaderPool, io::ReadFile("assets/debug_fs.glsl"), CGShaderType::Fragment) };

	int32_t shaderProgramIndex = ShaderOps::CreateShaderProgram(info.rendererType, context.shaderPool, 2, shaderIndices);

	printf("Vertex shader index: %d\n", shaderIndices[0]);
	printf("Fragment shader index: %d\n", shaderIndices[1]);
	printf("Shader program index: %d", shaderProgramIndex);

	const uint32_t vertexBuffer = context.bufferPool.vertexArrays[vertexBufferIndex];
	const uint32_t shaderProgram = context.shaderPool.shaderPrograms[shaderProgramIndex];

	while (engine.IsRunning())
	{
		cg::PollEvents();

		renderer.ClearView(CG_CLEAR_COLOR, 0xFF0000FF);

		renderer.BindVertexBuffer(vertexBuffer);
		renderer.DrawElements(6);
		renderer.Submit(shaderProgram);

		FrameOps::Present(context.apiFunctions, engine.GetWindow().winptr);
	}

	return 0;
}
