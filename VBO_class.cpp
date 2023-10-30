#include"VBO_class.h"

VBO::VBO(std::initializer_list<Vertex> coords) : vertices(coords) {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &id);
}

VBO::~VBO(){
	if (id != -1) {
		glDeleteProgram(id);
	}
}

VBO::VBO(VBO&& move) noexcept {
	std::swap(id, move.id);
}

VBO& VBO::operator=(VBO&& other) noexcept {
	std::swap(id, other.id);
	return *this;
}