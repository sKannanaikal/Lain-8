#ifndef GRAPHICS_SHADERS_HPP
#define GRAPHICS_SHADERS_HPP

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Lain8::Graphics {

class Shader {
public:
	GLuint shader_program;
	Shader(const char *vertexFile, const char *fragmentFile);

	void activate_shader();
	void delete_shader();
};

}

#endif /* GRAPHICS_SHADERS_HPP */

