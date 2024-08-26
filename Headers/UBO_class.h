#ifndef UNIBUFFER_CLASS_H
#define UNIBUFFER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "iostream"

#include <glm/glm.hpp>

class UniformBuffer {
public:
	UniformBuffer() = default;
	UniformBuffer(unsigned int size);

	void Bind();
	void Unbind();

	void SubData(const unsigned int position, const unsigned int size, glm::mat4* value);

	unsigned int GetId();

	UniformBuffer(UniformBuffer&& move) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;

	~UniformBuffer();
//private:
	GLuint ubo_id = NULL;
};

#endif