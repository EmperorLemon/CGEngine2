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

    while (engine.IsRunning())
    {
        cgengine::PollEvents();

        renderer.api.Clear(cgengine::renderer::CGClearFlags::Color);
        renderer.api.ClearColor(0.5f, 0.2f, 0.6f, 0.5f);

        engine.GetContext().SwapBuffers();
    }

    return EXIT_SUCCESS;
}