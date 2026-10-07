cmake_minimum_required(VERSION 3.31)
project(Lain-8)

# Setting up Directory and Compiler Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Loading in external Libraries
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v1.18.0)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Building out External Libraries
add_subdirectory(external/glfw)
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Building out Local Libraries
add_subdirectory(src/core)
add_subdirectory(src/graphics)
target_link_libraries(graphics_lib PUBLIC glad)
target_link_libraries(core_lib PRIVATE graphics_lib) #remove this in final build no dependcey needed
# Building out main emulator application
add_executable(lain8 src/main.cpp)

target_link_libraries(lain8 PRIVATE core_lib graphics_lib glfw)

# Building out tester binary for unit tests
add_executable(lain8-tester test/main.cpp)
target_link_libraries(lain8-tester GTest::gtest_main)
