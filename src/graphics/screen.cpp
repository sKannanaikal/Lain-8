#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "screen.hpp"

namespace Lain8::Graphics {

Screen::Screen() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->window = glfwCreateWindow(640, 320, "Lain8", NULL, NULL);
	if (this->window == NULL) {
		std::println("Failed to create GLFW window");
		glfwTerminate();
		return;
	}
}

void Screen::turn_on() {
	glfwMakeContextCurrent(this->window);

	gladLoadGL();
	glViewport(0, 0, 800, 800);

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

void Screen::render(std::uint8_t screen_buffer[]) {
	return;
}

void Screen::turn_off() {
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

}
