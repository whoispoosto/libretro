cmake_minimum_required(VERSION 4.0)   # Specify minimum CMake version
project(MyProject CXX)                # Define project name and language

set(CMAKE_CXX_STANDARD 23)            # Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # Require the specified C++ standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(bin main.cpp window.cpp)    # Create an executable target named MyProject from main.cpp

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)

target_link_libraries(bin PRIVATE 
  glfw
  OpenGL::GL,
  glad
)

add_definitions(-DGL_SILENCE_DEPRECATION)


