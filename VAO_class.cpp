#include"VAO_class.h"

VAO::VAO(VBO& VBO, GLuint layout) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(id);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteBuffers(1, &id);
}

VAO::~VAO() {
	if (id != -1) {
		glDeleteVertexArrays(1, &id);
	}
}

VAO::VAO(VAO&& move) noexcept{
	std::swap(id, move.id);
}

VAO& VAO::operator=(VAO&& other) noexcept{
	std::swap(id, other.id);
	return *this;
}