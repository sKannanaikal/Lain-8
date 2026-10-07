add_library(graphics_lib STATIC
	EBO.cpp
	VBO.cpp
	VAO.cpp
	shaders.cpp
	screen.cpp
)

target_include_directories(graphics_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
