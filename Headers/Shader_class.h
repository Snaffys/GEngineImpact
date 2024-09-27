#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// For swapping
#include <stdio.h>

#include <cstdio>

// Read shader files
#include <fstream>

// Handle errors
#include <cerrno>

// Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	Shader() = default;
	Shader(const char* vertex_file, const char* fragment_file);
	Shader(const char* vertex_file, const char* fragment_file, const char* geometry_file);

	Shader(Shader&& move) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void Activate();
	void Delete();

	void Uniform_color(const GLchar* u_name);
	void Uniform_position(const GLchar* u_name);

	void setInt(const GLchar* name, int value);
	void setFloat(const GLchar* name, float value);

	void Uniform_matrix(const GLchar* name, glm::mat4& trans);

	void setVec3(const GLchar* name, glm::vec3& values);
	void setVec3(const GLchar* name, float firest_value, float second_value, float third_value);

	void setMat4(const GLchar* name, const glm::mat4& mat);

	void setBool(const GLchar* name, bool value);

	unsigned int GetID();

	~Shader();
private:
	char* get_file_contents(const char* filename);

	int success;
	char info_log[512];

	int compile_shader(GLuint type, const char* filename);

	GLuint vertex_shader = NULL, fragment_shader = NULL, geometry_shader = NULL, id = NULL;
};

#endif