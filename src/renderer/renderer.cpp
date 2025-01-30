#include "renderer.h"

// renderer.cpp
namespace cg::renderer
{
	void CGRenderer::Clear(const CGClearFlags flags) const 
	{ 
		if (m_api.Clear) 
			m_api.Clear(flags); 
	}
	
	void CGRenderer::ClearColor(const float r, const float g, const float b, const float a) const 
	{ 
		if (m_api.ClearColor)
			m_api.ClearColor(r, g, b, a); 
	}
	
	void CGRenderer::ClearColor(const uint32_t color) const
	{
		float r = ((color >> 24) & 0xFF) / 255.0f;
		float g = ((color >> 16) & 0xFF) / 255.0f;
		float b = ((color >> 8) & 0xFF) / 255.0f;
		float a = (color & 0xFF) / 255.0f;

		if (m_api.ClearColor)
			m_api.ClearColor(r, g, b, a);
	}
}