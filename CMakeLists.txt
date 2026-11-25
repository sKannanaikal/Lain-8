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
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

# Building out Local Libraries
add_subdirectory(src/core)
target_link_libraries(core_lib SDL3::SDL3)

# Building out main emulator application
add_executable(lain8 src/main.cpp)
target_link_libraries(lain8 PRIVATE core_lib)

# Building out tester binary for unit tests
add_executable(lain8-tester tests/testmain.cpp src/core/emulator.cpp)
target_include_directories(lain8-tester PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(lain8-tester PRIVATE GTest::gtest_main SDL3::Headers)
include(GoogleTest)
gtest_discover_tests(lain8-tester)
