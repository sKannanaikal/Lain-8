#ifndef GRAPHICS_EBO_HPP
#define GRAPHICS_EBO_HPP


#include <glad/glad.h>

namespace Lain8::Graphics {

class EBO {
public:
	GLuint element_buffer_object;
	EBO(GLuint *indices);
	void activate_element_buffer();
	void deactivate_element_buffer();
	void remove_element_buffer();
};

}

#endif /* GRAPHICS_EBO_HPP */
