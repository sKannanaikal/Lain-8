#include "shaders.hpp"

namespace Lain8::Graphics {

std::string _get_file_contents(const char *filename) {
	std::ifstream input(filename, std::ios::binary);
	std::string contents = "";
	if (input) {
		input.seekg(0, std::ios::end);
		contents.resize(input.tellg());

		input.seekg(0, std::ios::beg);
		input.read(&contents[0], contents.size());
		input.close();
	}

	return contents;
}

Shader::Shader(const char* vertexFile, const char *fragmentFile) {
	std::string vertex_source = _get_file_contents(vertexFile);
	std::string fragment_source = _get_file_contents(fragmentFile);

	const char *vertex_shader_source = vertex_source.c_str();
	const char *fragment_shader_source = fragment_source.c_str();

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::activate_shader() {
	glUseProgram(shader_program);
}

void Shader::delete_shader() {
	glDeleteProgram(shader_program);
}

}
