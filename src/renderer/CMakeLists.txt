set(RENDERER
	renderer/defines.h

	renderer/context/context.hpp
	renderer/context/context.cpp
	renderer/context/context_d3d11.h
	renderer/context/context_d3d11.cpp
	renderer/context/context_opengl.h
	renderer/context/context_opengl.cpp

	renderer/buffer/buffer.h

	renderer/renderer.h
	renderer/renderer.cpp
	renderer/renderer_d3d11.cpp
	renderer/renderer_opengl.cpp
	
	PARENT_SCOPE
)