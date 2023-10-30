#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"VBO_class.h"

class VAO {
public:
	VAO() = default;

	VAO(VBO& VBO, GLuint layout);
	VAO(VAO&& move) noexcept;
	~VAO();
	VAO& operator=(VAO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();
private:
	GLuint id = -1;	
};

#endif