#include "Shader_class.h"

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
		printf("Shaders linking failed\n%s\n", info_log);
	}

	// Frees the memory associated with shaders (flagging shaders for deletion)
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}



Shader::Shader(const char* vertex_file, const char* fragment_file, const char* geometry_file) {
	// Compiles vertex and fragment shaders
	vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_file);
	fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_file);
	geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_file);

	// Creates an empty object program for shaders
	id = glCreateProgram();
	// Attaches shaders
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	glAttachShader(id, geometry_shader);
	// Links program
	glLinkProgram(id);

	// Checks for errors
	glGetShaderiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, info_log);
		printf("Shaders linking failed\n%s\n", info_log);
	}

	// Frees the memory associated with shaders (flagging shaders for deletion)
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteShader(geometry_shader);
}


int Shader::compile_shader(GLuint type, const char* filename) {
	// Gets pointer to the array that contains GLSL code
	const char* shader_source = get_file_contents(filename);
	// Creates and compiles the shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	// Checks for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		printf(type == GL_VERTEX_SHADER ? "Vertex " : GL_FRAGMENT_SHADER ? "Fragment " : "Geometry ");
		printf("shader compilation failed\n%s\n", info_log);
	}
	return shader;
}

// Converts GLSL code into array of chars
char* Shader::get_file_contents(const char* filename) {
	std::ifstream stream(filename, std::ios::binary);
	if (stream) {
		stream.seekg(0, std::ios::end);
		int arrSize = stream.tellg() * sizeof(char) + 1;
		char* contents = (char*)malloc(arrSize);
		contents[arrSize - 1] = '\0';
		stream.seekg(0, std::ios::beg);
		stream.read(contents, arrSize);
		stream.close();
		return(contents);
	}
	throw(errno);
}

void Shader::Activate() {
	glUseProgram(id);
}

// Throws data stream the sheaders
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

unsigned int Shader::GetID() {
	return id;
}

void Shader::setInt(const GLchar* name, int value) {
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setFloat(const GLchar* name, float value) {
	glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::Uniform_matrix(const GLchar* name, glm::mat4& trans) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::setVec3(const GLchar* name, glm::vec3& values) {
	glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(values));
}

void Shader::setVec3(const GLchar* name, float first_value, float second_value, float third_value) {
	glUniform3f(glGetUniformLocation(id, name), first_value, second_value, third_value);
}

void Shader::setMat4(const GLchar* name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
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
	std::swap(geometry_shader, move.geometry_shader);
	std::swap(success, move.success);
	std::swap(info_log, move.info_log);
}

Shader& Shader::operator=(Shader&& other) noexcept {
	std::swap(id, other.id);
	std::swap(vertex_shader, other.vertex_shader);
	std::swap(fragment_shader, other.fragment_shader);
	std::swap(geometry_shader, other.geometry_shader);
	std::swap(success, other.success);
	std::swap(info_log, other.info_log);
	return *this;
}