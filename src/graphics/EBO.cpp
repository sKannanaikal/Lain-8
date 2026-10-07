
#include "EBO.hpp" //todo update this to use the <> notation in CMakeLists.txt

namespace Lain8::Graphics {

EBO::EBO(GLuint *indices) {
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void EBO::activate_element_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, element_buffer_object);
}

void EBO::deactivate_element_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EBO::remove_element_buffer() {
	glDeleteBuffers(1, &element_buffer_object);
}

}
