add_library(core_lib STATIC emulator.cpp)
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(frontend_lib STATIC monitor.cpp session.cpp shell.cpp)
target_link_libraries(frontend_lib PUBLIC core_lib PRIVATE SDL3::SDL3)
