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
	CGEngineCreateInfo info = {};
	info.rendererType = CGRendererType::Direct3D11;
	info.resolution.width = WINDOW_WIDTH;
	info.resolution.height = WINDOW_HEIGHT;
	info.debug = true;

	CGEngine engine(info);
	CGRenderer& renderer = engine.GetRenderer();
	[[maybe_unused]] core::CGWindow& window = engine.GetWindow();

	const CGRenderCommand commands[] = 
	{ 
		{ RenderOps::SetViewClear(0, CG_CLEAR_COLOR, CG_BLUE) }
	};

	const int8_t count = sizeof(commands) / sizeof(commands[0]);

	AddRenderCommands(renderer, count, commands);

	while (engine.IsRunning())
	{
		cg::PollEvents();

		ExecuteRenderCommands(renderer);

		FrameOps::Present(renderer);
	}

	return 0;
}
