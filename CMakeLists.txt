cmake_minimum_required(VERSION 3.31)
project(Lain-8)

# Setting up Directory and Compiler Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

# Loading in external Libraries
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v1.18.0)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Building out Local Libraries
add_subdirectory(src/general)

# Building out main emulator application
add_executable(lain8 src/main.cpp)
target_link_libraries(lain8 general_lib)

# Building out tester binary for unit tests
add_executable(lain8-tester test/main.cpp)
target_link_libraries(lain8-tester GTest::gtest_main)
