set(RENDERER
	renderer/defines.h

	renderer/buffer/layout.h
	renderer/buffer/buffer.h
	renderer/buffer/buffer.cpp
	renderer/buffer/buffer_opengl.cpp

	renderer/context/context.hpp
	renderer/context/context.cpp
	renderer/context/context_d3d11.h
	renderer/context/context_d3d11.cpp
	renderer/context/context_opengl.h
	renderer/context/context_opengl.cpp

	renderer/renderer.h
	renderer/renderer.cpp
	renderer/renderer_d3d11.cpp
	renderer/renderer_opengl.cpp

	renderer/shader/shader.h
	renderer/shader/shader.cpp
	renderer/shader/shader_opengl.cpp
	
	PARENT_SCOPE
)