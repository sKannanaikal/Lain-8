#include "VAO.hpp"

namespace Lain8::Graphics {

VAO::VAO() {
	glGenVertexArrays(1, &vertex_array_object);
}

void VAO::LinkVBO(VBO &vertex_buffer_object, GLuint layout) {
	vertex_buffer_object.activate_vertex_buffer();
	glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(layout);
	vertex_buffer_object.deactivate_vertex_buffer();
}

void VAO::activate_vertex_array() {
	glBindVertexArray(vertex_array_object);
}

void VAO::deactivate_vertex_array() {
	glBindVertexArray(0);
}

void VAO::remove_vertex_array() {
	glDeleteVertexArrays(1, &vertex_array_object);
}

}
