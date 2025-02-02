#pragma once

#include <cstdint>

// window.h
namespace cg
{
	enum class CGRendererType : uint8_t;

	namespace core
	{
		struct CGWindow
		{
			void* winptr = nullptr; // Window handle for window libraries (GLFW, SDL, etc.)
			void* nwh = nullptr; // Platform-specific window handle (HWND, NSWindow*, etc.)
			int32_t width = 0;
			int32_t height = 0;
		};
	}

	bool CreateCGWindow(int32_t width, int32_t height, const char* title, core::CGWindow& window);
	void DestroyCGWindow(void* winptr);

	void PollEvents();
	bool ShouldCloseCGWindow(void* winptr);
}

