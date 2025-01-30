#pragma once

#include <cstdint>

// window.h
namespace cg
{
	enum class CGRendererType : uint32_t;

	namespace core
	{
		enum class CGWindowFlags : uint32_t
		{
			None = 0,
			Resizable = 1 << 0,
			Fullscreen = 1 << 1,
			Borderless = 1 << 2,
			// etc...
		};

		struct CGWindow
		{
			int32_t width = 0;
			int32_t height = 0;
			void* winptr = nullptr; // Window handle for window libraries (GLFW, SDL, etc.)
			void* nwh = nullptr; // Platform-specific window handle (HWND, NSWindow*, etc.)
		};
	}

	bool CreateCGWindow(CGRendererType type, bool debug, int32_t width, int32_t height, const char* title, core::CGWindow& window);
	void DestroyCGWindow(void* winptr);

	void PollEvents();
	bool ShouldCloseCGWindow(void* winptr);
}