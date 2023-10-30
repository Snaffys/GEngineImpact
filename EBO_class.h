#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <initializer_list>

struct Index {
	int first, second, third;
};

class EBO {
public:
	EBO() = default;

	//Buffers(Buffers const&) = default;

	EBO(std::initializer_list<Index> index);

	EBO(EBO&& move) noexcept;

	EBO& operator=(EBO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~EBO();
private:
	std::vector<Index> indeces;

	GLuint id = -1;
};

#endif