#pragma once

#include "context/context.h"

// renderer.h
namespace cg::renderer
{
	constexpr uint16_t CG_MAX_RENDER_COMMANDS = 1024;

	enum CGColor : uint32_t
	{
		CG_BLACK = 0x000000FF,
		CG_WHITE = 0xFFFFFFFF,
		CG_RED = 0xFF0000FF,
		CG_GREEN = 0x00FF00FF,
		CG_BLUE = 0x0000FFFF,
	};

	enum CGClearFlags : uint16_t
	{
		CG_CLEAR_COLOR = 0x1,
		CG_CLEAR_DEPTH = 0x1 << 1,
		CG_CLEAR_STENCIL = 0x1 << 2
	};

	enum CGRenderCommandType : uint32_t
	{
		CG_NONE = 0,
		CG_CLEAR = 1,
		CG_SET_VERTEX_BUFFER = 2,
		CG_SET_INDEX_BUFFER = 3,
		CG_DRAW_VERTICES = 4,
		CG_DRAW_INDEXED = 5,
		CG_SUBMIT = 6,
	};

	struct alignas(16) CGRenderCommand
	{
		CGRenderCommandType commandType = CGRenderCommandType::CG_NONE;
		uint8_t padding[4] = {};

		union {
			struct {
				uint32_t color;
				CGClearFlags clearFlags;
				int16_t view;
			} clear;
			struct {
				uint32_t vertexBuffer;
			} setVertexBuffer;
			struct {
				int32_t count;
			} drawIndexed;
			struct {
				uint32_t program;
				int16_t view;
			} submit;
		} params;
	};

	struct CGRenderer
	{
		CGRenderer(CGRenderContext& ctx);

		CGRenderCommand renderCommands[CG_MAX_RENDER_COMMANDS] = {};
		CGRenderContext& renderContext;
		int16_t renderCommandCount = 0;
	};

	bool AddRenderCommands(CGRenderer& renderer, int16_t count, const CGRenderCommand commands[]);
	void SubmitRenderCommands(const CGRenderer& renderer);

	namespace RenderOps
	{
		CGRenderCommand ClearView(int16_t view, CGClearFlags flags, uint32_t color);
		CGRenderCommand SetVertexBuffer(uint32_t vertexBuffer);
		CGRenderCommand DrawIndexed(int32_t count);
		CGRenderCommand Submit(int16_t view, uint32_t program);

		namespace OpenGL
		{
			void OpenGLClearView(CGClearFlags flags, float r, float g, float b, float a);
			void OpenGLBindVertexArray(uint32_t vertexArray);
			void OpenGLDrawElements(int32_t count);
			void OpenGLUseProgram(uint32_t program);
		}

		namespace D3D11
		{
			void D3D11ClearView(void* deviceCtx, void* renderTV, CGClearFlags flags, float r, float g, float b, float a);
		}
	}
}
