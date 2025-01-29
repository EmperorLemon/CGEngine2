#pragma once

#include <cstdint>
#include <functional>

// context.h

namespace cgengine
{
	enum class CGRendererType : uint32_t;

	namespace core
	{
		struct CGWindow;
	}
}

namespace cgengine::renderer
{
	struct CGRenderContext
	{
		std::function<bool()> Init = nullptr;
		std::function<void()> Shutdown = nullptr;
		std::function<void()> SwapBuffers = nullptr;
		std::function<void()> BeginFrame = nullptr;
		std::function<void()> EndFrame = nullptr;
		void* userData = nullptr;
	};

	void OpenGLInit(CGRenderContext& context);
	void D3D11Init(CGRenderContext& context);

	void CreateOpenGLContext(void* winptr, CGRenderContext& context);
	void CreateD3D11Context(CGRenderContext& context);
}