#ifndef GRAPHICS_VAO_HPP
#define GRAPHICS_VAO_HPP

#include <glad/glad.h>

#include "VBO.hpp"

namespace Lain8::Graphics {

class VAO {

public:
	GLuint vertex_array_object;
	VAO();
	void LinkVBO(VBO &vertex_buffer_object, GLuint layout);
	void activate_vertex_array();
	void deactivate_vertex_array();
	void remove_vertex_array();

};

}

#endif /* GRAPHICS_VAO_HPP */
