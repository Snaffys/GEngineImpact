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

	void MatInstance(const int index, VBO& VBO);
	void VecInstance(int index, VBO& VBO);
	void Bind();
	void Unbind();
	void Delete();

	~VAO();
protected:
	GLuint id = NULL;
};


class CubemapVAO : public VAO {
public:
	CubemapVAO() = default;
	CubemapVAO(VBO& VBO);
};

class FramebufferVAO : public VAO {
public:
	FramebufferVAO() = default;
	FramebufferVAO(VBO& VBO);
};

class NormalVAO : public VAO {
public:
	NormalVAO() = default;
	NormalVAO(VBO& VBO);
};

#endif