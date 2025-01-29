#include <cstdlib>
#include <cstdio>

#include "cgengine.h"
#include "renderer/renderer.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// main.cpp
int main(int argc, char* argv[])
{
    cgengine::CGEngineCreateInfo info;
    info.type = cgengine::CGRendererType::OpenGL;
    info.resolution.width = WINDOW_WIDTH;
    info.resolution.height = WINDOW_HEIGHT;
    info.debug = true;

    cgengine::CGEngine engine(info);

    const auto& renderer = engine.GetRenderer();
    const auto& context = engine.GetContext();

    while (engine.IsRunning())
    {
        cgengine::PollEvents();

        renderer.ClearColor(0.5f, 0.2f, 0.6f, 1);
        renderer.Clear(cgengine::renderer::CGClearFlags::Color);

        context.SwapBuffers();
    }

    return EXIT_SUCCESS;
}