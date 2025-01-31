#include <cstdlib>
#include <cstdio>

#include "cgengine.h"
#include "core/logger.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// main.cpp
int main(int argc, char* argv[])
{
    cg::CGEngineCreateInfo info;
    info.rendererType = cg::CGRendererType::Direct3D11;
    info.resolution.width = WINDOW_WIDTH;
    info.resolution.height = WINDOW_HEIGHT;
    info.debug = true;

    cg::CGEngine engine(info);

    const auto& renderer = engine.GetRenderer();
    const auto& context = engine.GetRenderContext();

    while (engine.IsRunning())
    {
        cg::PollEvents();
        
        renderer.ClearView(CG_CLEAR_COLOR, 0xFF0000FF);

        context.Present();
    }

    return EXIT_SUCCESS;
}