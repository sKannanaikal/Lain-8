#ifndef GRAPHICS_VBO_HPP
#define GRAPHICS_VBO_HPP

#include <glad/glad.h>

namespace Lain8::Graphics {

class VBO {
public:
	GLuint vertex_buffer_object;
	VBO(GLfloat *vertices);
	void activate_vertex_buffer();
	void deactivate_vertex_buffer();
	void remove_vertex_buffer();
};

}
#endif /* GRAPHICS_VBO_HPP */
