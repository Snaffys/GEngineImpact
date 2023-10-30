#include "Shader_class.h"

std::string Shader::get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

int Shader::compile_shader(GLuint type, const char* filename) {
	//gets code on GLSL in string format
	std::string shader_code = get_file_contents(filename);
	//gets pointer to an array that contains GLSL code
	const char* shader_source = shader_code.c_str();
	//compiles the shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	//checking
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << (type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ") << "shader compilation failed\n" << info_log << std::endl;
	}
	return shader;
}

Shader::Shader(const char* vertex_file, const char* fragment_file) {

	vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_file);
	fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_file);

	id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	glLinkProgram(id);

	//Checking
	glGetShaderiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, info_log);
		std::cout << "Shaders linking failed\n" << info_log << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::Activate() {
	glUseProgram(id);
}

void Shader::Delete() {
	glDeleteProgram(id);
}

Shader::~Shader() {
	if (id != -1) {
		glDeleteProgram(id);
	}
}

Shader::Shader(Shader&& move) noexcept {
	std::swap(id, move.id);
	std::swap(vertex_shader, move.vertex_shader);
	std::swap(fragment_shader, move.fragment_shader);
	std::swap(success, move.success);
	std::swap(info_log, move.info_log);
}

Shader& Shader::operator=(Shader&& other) noexcept {
	std::swap(id, other.id);
	std::swap(vertex_shader, other.vertex_shader);
	std::swap(fragment_shader, other.fragment_shader);
	std::swap(success, other.success);
	std::swap(info_log, other.info_log);
	return *this;
}