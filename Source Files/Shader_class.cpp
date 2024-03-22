#include "Shader_class.h"

// Converts GLSL code
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
	// Gets code on GLSL in string format
	std::string shader_code = get_file_contents(filename);
	// Gets pointer to an array that contains GLSL code
	const char* shader_source = shader_code.c_str();
	// Compiles the shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	// Checks for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << (type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ") << "shader compilation failed\n" << info_log << std::endl;
	}
	return shader;
}

Shader::Shader(const char* vertex_file, const char* fragment_file) {
	// Compiles vertex and fragment shaders
	vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_file);
	fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_file);

	// Creates an empty object program for shaders
	id = glCreateProgram();
	// Attaches shaders
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	// Links program
	glLinkProgram(id);

	// Checks for errors
	glGetShaderiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, info_log);
		std::cout << "Shaders linking failed\n" << info_log << std::endl;
	}

	// Frees the memory associated with shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::Activate() {
	glUseProgram(id);
}

// Throws data in the sheaders
// color changing
void Shader::Uniform_color(const GLchar* u_name) {
	float time_value = glfwGetTime();
	float red_color_value = (sin(time_value) / 2.0f) + 0.5f;
	float green_color_value = (sin(time_value / 2.0f) / 2.0f) + 0.5f;
	float blue_color_value = (sin(time_value / 4.0f) / 2.0f) + 0.5f;
	glUniform4f(glGetUniformLocation(id, u_name), red_color_value, green_color_value, blue_color_value, 1.0f);
}
// position changing
void Shader::Uniform_position(const GLchar* u_name) {
	float time_value = glfwGetTime();
	float position_value = (sin(time_value) / 2.0f);
	glUniform2f(glGetUniformLocation(id, u_name), position_value, position_value);
}

void Shader::setInt(const GLchar* name, int value) {
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setFloat (const std::string &name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::Uniform_matrix (const GLchar* name, glm::mat4& trans) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::setVec3(const std::string &name, glm::vec3& values) {
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(values));
}

void Shader::setVec3(const std::string &name, float first_value, float second_value, float third_value) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), first_value, second_value, third_value);
}

void Shader::setMat4(const std::string &name, const glm::mat4& mat){
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Delete() {
	glDeleteProgram(id);
}

Shader::~Shader() {
	if (id != -1)
		glDeleteProgram(id);
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