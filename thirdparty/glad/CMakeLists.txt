add_library(glad 
	include/glad/gl.h 
	src/gl.c
)

target_include_directories(glad SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)