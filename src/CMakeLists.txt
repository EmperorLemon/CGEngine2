add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)

target_sources(${PROJECT_NAME} 
	PUBLIC
		types.h
	PRIVATE 
		${CORE}
		${PLATFORM}
		${RENDERER}

		cgengine.h
		cgengine.cpp

		main.cpp
)