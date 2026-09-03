add_library(general_lib STATIC resources.cpp)
target_include_directories(general_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
