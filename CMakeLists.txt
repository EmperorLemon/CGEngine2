cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME CGEngine2)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Add executable
add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(thirdparty)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        glad
        glfw
        d3d11.lib
        dxgi.lib
)
