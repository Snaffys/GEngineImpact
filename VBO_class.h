#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <initializer_list>

struct Vertex {
	float x, y, z;
};

class VBO{
public:
	VBO() = default;

	//Buffers(Buffers const&) = default;

	VBO(std::initializer_list<Vertex> coords);
	VBO(VBO&& move) noexcept;

	VBO& operator=(VBO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~VBO();
private:
	std::vector<Vertex> vertices;

	GLuint id = -1;
};

#endif