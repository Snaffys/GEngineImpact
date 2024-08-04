#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <initializer_list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Index {
	Index() = default;

	Index(unsigned int first, unsigned int second, unsigned int third) : indices(first, second, third) {};

	glm::uvec3 indices;

	Index(Index&& move) noexcept {
		std::swap(indices, move.indices);
	}

	Index(const Index& copy) noexcept {
		indices = copy.indices;
	}

	Index& operator=(Index&& other) noexcept {
		std::swap(indices, other.indices);
		return *this;
	}
};

class EBO {
public:
	EBO() = default;

	EBO(std::initializer_list<Index> index);

	EBO(const std::vector<unsigned int>& input_indices);

	EBO(EBO&& move) noexcept;
	EBO& operator=(EBO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~EBO();
private:
	std::vector<Index> indices;

	GLuint id = NULL;
};

#endif