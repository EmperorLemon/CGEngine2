add_subdirectory(io)
add_subdirectory(platform)
add_subdirectory(renderer)

target_sources(${PROJECT_NAME} 
	PRIVATE 
		types.h

		${IO}
		${PLATFORM}
		${RENDERER}

		pch.h

		cgengine.h
		cgengine.cpp

		main.cpp
)