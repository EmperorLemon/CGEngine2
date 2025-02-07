add_subdirectory(io)
add_subdirectory(platform)
add_subdirectory(renderer)

target_sources(${PROJECT_NAME} 
	PRIVATE 
		${IO}
		${PLATFORM}
		${RENDERER}

		cgengine.h
		cgengine.cpp

		main.cpp
)