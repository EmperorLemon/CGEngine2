cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME CGEngine2)
project(${PROJECT_NAME} LANGUAGES C CXX)

# Language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Ensures -std=c++17 instead of -std=gnu++17

# Build settings
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Output directories (can be simplified)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Dependencies
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

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
        d3d11.lib
        dxgi.lib
        glad
        glfw
)

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -fcolor-diagnostics
        -Wno-c++98-compat
        -Wno-covered-switch-default
        -Wno-c++98-compat-pedantic
        -Wno-padded
        -Wno-deprecated-declarations
        -Wno-exit-time-destructors
        -Wno-switch-enum
        -Wno-unsafe-buffer-usage
        # GPU-friendly optimizations
        $<$<CONFIG:Release>:
            -march=native
            -mtune=native
            -ffast-math
            -fomit-frame-pointer>
    >
    
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /MP     # Multi-threaded compilation
        /GR-    # Disable RTTI
        /EHsc   # Exception handling
        $<$<CONFIG:Release>:
            /O2     # Max optimization
            /Oi     # Intrinsics
            /GL     # Whole program optimization
            /Gy     # Function-level linking
        >
    >
)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:/LTCG>  # Link-time code generation
    >
)

# Definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    >
)

# Assets symlink
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:CGEngine2>/assets
)