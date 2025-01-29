add_subdirectory(platform)
add_subdirectory(renderer)

target_sources(${PROJECT_NAME} 
	PUBLIC
		types.h
	PRIVATE 
		${PLATFORM}
		${RENDERER}

		main.cpp
		cgengine.h
		cgengine.cpp
)