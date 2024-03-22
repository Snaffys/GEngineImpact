#pragma once
#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include"VBO_class.h"

class VAO {
public:
	VAO() = default;

	VAO(VBO& VBO);

	VAO(VAO&& move) noexcept;
	VAO& operator=(VAO&& other) noexcept;

	void FramebufferVAO(VBO& VBO);
	void Bind();
	void Unbind();
	void Delete();

	~VAO();
private:
	GLuint id = -1;	
};

#endif