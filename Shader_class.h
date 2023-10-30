#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

class Shader {
public:
	Shader() = default;
	
	Shader(const char* vertex_file, const char* fragment_file);
	
	Shader(Shader&& move) noexcept;

	Shader& operator=(Shader&& other) noexcept;

	void Delete();
	~Shader();
	void Activate();
private:
	int compile_shader(GLuint type, const char* filename);
	GLuint vertex_shader = -1, fragment_shader = -1, id = -1;
	std::string get_file_contents(const char* filename);
	int success;
	char info_log[512];

};

#endif