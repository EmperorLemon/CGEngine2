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
static void CreateVertexBuffer(CGRenderer& renderer, CGShader& vShader, CGVertexLayout& vLayout, CGBuffer& vBuffer);
static void CreateIndexBuffer(CGRenderer& renderer, CGBuffer& iBuffer);
static void CreateFragmentShader(CGRenderer& renderer, CGShader& fShader);
static void CreateShaderProgram(const uint8_t shaderCount, const CGShader shaders[], CGRenderer& renderer, uint32_t& program);

void CreateViewport(const core::CGWindow& window, CGRenderer& renderer)
{
	ContextOps::CreateViewport(window, renderer);
}

void CreateVertexBuffer(CGRenderer& renderer, CGShader& vShader, CGVertexLayout& vLayout, CGBuffer& vBuffer)
{
	CGVertexElement elements[] =
	{
		{ CGVertexAttribute::Position, CGVertexFormat::Float3 },
		{ CGVertexAttribute::Color, CGVertexFormat::Float4 }
	};

	const uint8_t count = sizeof(elements) / sizeof(elements[0]);

	if (!DeviceOps::SetupVertexLayout(count, elements, renderer, vLayout))
	{
		printf("\nVertex layout setup failed\n");
	}

	CGBufferDesc vbDesc = {};
	vbDesc.type = CGBufferType::Vertex;
	vbDesc.usage = CGBufferUsage::Static;
	vbDesc.count = sizeof(vertices) / sizeof(vertices[0]);
	vbDesc.stride = vLayout.size;
	vbDesc.size = vbDesc.count * sizeof(float);

	if (!DeviceOps::CreateVertexBuffer(vbDesc, renderer, vBuffer, vertices))
	{
		printf("\nVertex buffer failed\n");
	}

	if (!DeviceOps::CreateVertexLayout(vBuffer, renderer, vShader, vLayout))
	{
		printf("\nVertex layout failed\n");
	}
}

void CreateIndexBuffer(CGRenderer& renderer, CGBuffer& iBuffer)
{
	CGBufferDesc ibDesc = {};
	ibDesc.type = CGBufferType::Index;
	ibDesc.usage = CGBufferUsage::Static;
	ibDesc.count = sizeof(indices) / sizeof(indices[0]);
	ibDesc.size = ibDesc.count * sizeof(uint16_t);

	if (!DeviceOps::CreateIndexBuffer(ibDesc, renderer, iBuffer, indices))
	{
		printf("\nIndex buffer failed\n");
	}
}

void CreateVertexShader(CGRenderer& renderer, CGShader& vShader)
{
	CGShaderDesc vsDesc = {};
	vsDesc.shaderType = CGShaderType::Vertex;
	vsDesc.filename = "assets/debug_vs.hlsl";
	vsDesc.entryPoint = "VSMain";

	if (!DeviceOps::CreateShader(vsDesc, renderer, vShader))
	{
		printf("\nVertex shader failed\n");
	}
}

void CreateFragmentShader(CGRenderer& renderer, CGShader& fShader)
{
	CGShaderDesc fsDesc = {};
	fsDesc.shaderType = CGShaderType::Fragment;
	fsDesc.filename = "assets/debug_ps.hlsl";
	fsDesc.entryPoint = "PSMain";

	if (!DeviceOps::CreateShader(fsDesc, renderer, fShader))
	{
		printf("\nFragment shader failed\n");
	}
}

void CreateShaderProgram(const uint8_t shaderCount, const CGShader shaders[], CGRenderer& renderer, uint32_t& program)
{
	if (!DeviceOps::CreateShaderProgram(shaderCount, shaders, renderer, program))
	{
		printf("\nShader program failed\n");
	}
}

// main.cpp
int main()
{
	CGEngineCreateInfo info = {};
	info.rendererType = CGRendererType::Direct3D11;
	info.resolution.width = WINDOW_WIDTH;
	info.resolution.height = WINDOW_HEIGHT;
	info.debug = true;

	CGEngine engine(info);
	CGRenderer& renderer = engine.GetRenderer();
	core::CGWindow& window = engine.GetWindow();

	CGVertexLayout vLayout;
	CGBuffer vBuffer, iBuffer;
	CGShader vShader, fShader;
	uint32_t program = 0u;

	CreateViewport(window, renderer);
	CreateVertexShader(renderer, vShader);
	CreateVertexBuffer(renderer, vShader, vLayout, vBuffer);
	CreateIndexBuffer(renderer, iBuffer);
	CreateFragmentShader(renderer, fShader);

	const CGShader shaders[] = { vShader, fShader };
	const uint8_t shaderCount = sizeof(shaders) / sizeof(shaders[0]);

	CreateShaderProgram(shaderCount, shaders, renderer, program);

	const uint8_t vertexCount = 3u;

	const CGRenderCommand commands[] = 
	{ 
		{ ContextOps::SetViewClear(0u, 0u, CG_CLEAR_COLOR, CG_DARK_GRAY) },
		{ ContextOps::SetPipelineState(program) },
		{ ContextOps::SetVertexBuffer(0u) },
		{ ContextOps::SetIndexBuffer(0u) },
		{ ContextOps::SetVertexShader(0u) },
		{ ContextOps::SetFragmentShader(0u) },
		{ RenderOps::Draw(0u, vertexCount, 0u) }
	};

	const uint8_t cmdCount = sizeof(commands) / sizeof(commands[0]);

	AddRenderCommands(cmdCount, commands, renderer);

	while (engine.IsRunning())
	{
		PollEvents();

		ExecuteRenderCommands(renderer);

		FrameOps::EndFrame(renderer);

		FrameOps::Present(renderer);
	}

	return 0;
}
