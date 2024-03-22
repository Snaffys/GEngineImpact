#include"EBO_class.h"

EBO::EBO(std::initializer_list<Index> index) : indices(index) {
	// Generates buffer
	glGenBuffers(1, &id);
	// Binds buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	// Creates and initialize EBO data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

EBO::EBO(const std::vector<unsigned int>& input_indices){
	//indices.resize(input_indices.size() * sizeof(unsigned int) / sizeof(Index));
	indices.resize(input_indices.size());
	memcpy((unsigned int*)indices.data(), input_indices.data(), sizeof(unsigned int) * indices.size());
	// Generates buffer
	glGenBuffers(1, &id);
	// Binds buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	// Creates and initialize EBO data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

// Binds buffer
void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

// Unbinds buffer
void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes buffer
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