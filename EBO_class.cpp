#include"EBO_class.h"

EBO::EBO(std::initializer_list<Index> index) : indeces(index) {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeces.size() * sizeof(indeces[0]), &indeces[0], GL_STATIC_DRAW);
}

void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &id);
}

EBO::~EBO() {
	if (id != -1) {
		glDeleteBuffers(1, &id);
	}
}

EBO::EBO(EBO&& move) noexcept {
	std::swap(id, move.id);
}

EBO& EBO::operator=(EBO&& other) noexcept {
	std::swap(id, other.id);
	return *this;
}