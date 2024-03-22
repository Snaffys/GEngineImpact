#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#include <vector>
#include <initializer_list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	Shader() = default;
	
	Shader(const char* vertex_file, const char* fragment_file);
	
	Shader(Shader&& move) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void Activate();
	void Delete();

	void Uniform_color(const GLchar* u_name);
	void Uniform_position(const GLchar* u_name);

	void setInt(const GLchar* name, int value);
	void setFloat(const std::string &name, float value);

	void Uniform_matrix(const GLchar* name, glm::mat4& trans);

	void setVec3(const std::string &name, glm::vec3& values);
	void setVec3(const std::string &name, float firest_value, float second_value, float third_value);

	void setMat4(const std::string& name, const glm::mat4& mat);

	~Shader();
private:
	std::string get_file_contents(const char* filename);
	
	int success;
	char info_log[512];

	int compile_shader(GLuint type, const char* filename);

	GLuint vertex_shader = -1, fragment_shader = -1, id = -1;
};

#endif