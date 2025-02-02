#include "context.hpp"

// context.cpp
namespace cg::renderer
{
	namespace FrameOps
	{
		void Present(const CGRenderContextAPIFunctions& apiFunctions, void* window)
		{
			apiFunctions.present(window);
		}
	}
}
