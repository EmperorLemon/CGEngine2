#include <cstdio>

#include "cgengine.h"
#include "io/fileio.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr float vertices[21] =
{
	 0.0f,   0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	 0.45f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
};

//[[maybe_unused]] constexpr float vertices[12] = { 
//	 0.5f,  0.5f, 0.0f,							  
//	 0.5f, -0.5f, 0.0f,
//	-0.5f, -0.5f, 0.0f,
//	-0.5f,  0.5f, 0.0f
//};

[[maybe_unused]] constexpr uint16_t indices[6] = 
{
	0, 1, 3,
	1, 2, 3
};

using namespace cg;
using namespace cg::renderer;

static void CreateViewport(const core::CGWindow& window, CGRenderer& renderer);
static void CreateVertexShader(CGRenderer& renderer, CGShader& vShader);
static void CreateVertexBuffer(CGRenderer& renderer, CGShader& vShader);
static void CreatePixelShader(CGRenderer& renderer, CGShader& pShader);

void CreateViewport(const core::CGWindow& window, CGRenderer& renderer)
{
	ContextOps::CreateViewport(window, renderer);
}

void CreateVertexBuffer(CGRenderer& renderer, CGShader& vShader)
{
	CGVertexLayout vLayout;

	CGVertexElement elements[] =
	{
		{ CGVertexAttribute::Position, CGVertexFormat::Float3 },
		{ CGVertexAttribute::Color, CGVertexFormat::Float4 }
	};

	const uint8_t count = sizeof(elements) / sizeof(elements[0]);

	if (!DeviceOps::CreateVertexLayout(count, elements, renderer, vShader, vLayout))
	{
		printf("Vertex layout failed");
	}

	CGBufferDesc desc = {};
	desc.type = CGBufferType::Vertex;
	desc.usage = CGBufferUsage::Static;
	desc.count = sizeof(vertices) / sizeof(vertices[0]);
	desc.stride = vLayout.size;
	desc.size = desc.count * sizeof(float);

	if (!DeviceOps::CreateVertexBuffer(desc, renderer, vertices))
	{
		printf("Vertex buffer failed");
	}
}

void CreateVertexShader(CGRenderer& renderer, CGShader& vShader)
{
	CGShaderDesc vsDesc = {};
	vsDesc.shaderType = CGShaderType::Vertex;
	vsDesc.filename = L"assets/debug_vs.hlsl";
	vsDesc.entryPoint = "VSMain";

	if (!DeviceOps::CreateShader(vsDesc, renderer, vShader))
	{
		printf("Vertex shader failed");
	}
}

void CreatePixelShader(CGRenderer& renderer, CGShader& pShader)
{
	CGShaderDesc fsDesc = {};
	fsDesc.shaderType = CGShaderType::Fragment;
	fsDesc.filename = L"assets/debug_ps.hlsl";
	fsDesc.entryPoint = "PSMain";

	if (!DeviceOps::CreateShader(fsDesc, renderer, pShader))
	{
		printf("Pixel shader failed");
	}
}

// main.cpp
int main()
{
	CGEngineCreateInfo info = {};
	info.rendererType = CGRendererType::OpenGL;
	info.resolution.width = WINDOW_WIDTH;
	info.resolution.height = WINDOW_HEIGHT;
	info.debug = true;

	CGEngine engine(info);
	CGRenderer& renderer = engine.GetRenderer();
	[[maybe_unused]] core::CGWindow& window = engine.GetWindow();

	CGShader vShader, pShader;

	CreateViewport(window, renderer);
	CreateVertexShader(renderer, vShader);
	CreateVertexBuffer(renderer, vShader);
	CreatePixelShader(renderer, pShader);

	const uint8_t vertexCount = 3u;

	const CGRenderCommand commands[] = 
	{ 
		{ ContextOps::SetViewClear(0u, CG_CLEAR_COLOR, CG_BLUE) }
	};

	const int8_t cmdCount = sizeof(commands) / sizeof(commands[0]);

	AddRenderCommands(cmdCount, commands, renderer.resourcePool.commandPool);

	while (engine.IsRunning())
	{
		cg::PollEvents();

		ExecuteRenderCommands(renderer);

		FrameOps::Present(renderer);
	}

	return 0;
}
