cmake_minimum_required(VERSION 3.16)

project(MyProject VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# TODO: Get rid of GLOB and switch to add_executable
file(GLOB SRC_FILES src/*.cpp src/*.c external/*/*.c)

add_executable(bin ${SRC_FILES})

# Include directories - add external/glad and external/KHR separately
target_include_directories(bin PRIVATE 
    include 
    external
)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(bin PRIVATE
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}  # Required for GLAD on Linux
)

# Use target_compile_definitions instead of add_definitions
target_compile_definitions(bin PRIVATE 
    GL_SILENCE_DEPRECATION 
    GLFW_INCLUDE_NONE
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Collect all test files from tests/ directory
file(GLOB TEST_SOURCES "tests/*.cc")

add_executable(
  libretro_tests
  ${TEST_SOURCES}
)

target_link_libraries(
  libretro_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(libretro_tests)

