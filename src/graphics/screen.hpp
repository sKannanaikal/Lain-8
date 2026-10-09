#ifndef GRAPHICS_SCREEN_HPP
#define GRAPHICS_SCREEN_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Lain8::Graphics {

class Screen {

public:
	GLFWwindow *window;
	Screen();
	void turn_on();
	void render(std::uint8_t screen_buffer []);
	void turn_off();
};

}

#endif /* GRAPHICS_SCREEN_HPP */
