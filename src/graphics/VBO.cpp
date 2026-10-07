#include "VBO.hpp" //todo update this to use the <> notation in CMakeLists.txt

namespace Lain8::Graphics {

VBO::VBO(GLfloat *vertices) {
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void VBO::activate_vertex_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
}

void VBO::deactivate_vertex_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::remove_vertex_buffer() {
	glDeleteBuffers(1, &vertex_buffer_object);
}

}
