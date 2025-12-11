cmake_minimum_required(VERSION 3.16)

project(MyProject VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source files
file(GLOB SRC_FILES src/*.cpp src/*.c)

add_executable(bin ${SRC_FILES})

# Include directory for headers
target_include_directories(bin PRIVATE include)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(bin PRIVATE
    glfw
    OpenGL::GL
)

add_definitions(-DGL_SILENCE_DEPRECATION -DGLFW_INCLUDE_NONE)



