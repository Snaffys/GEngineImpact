#include"VBO_class.h"


VBO::VBO(std::initializer_list<Vertex> coords) : vertices(coords) {
	// Genereates buffers
	glGenBuffers(1, &id);
	// Binds buffers
	Bind();
	// Creates and initialize VBO data store
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

VBO::VBO(std::initializer_list<VertexFramebuffer> coords) : vertices_fb(coords){
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices_fb.size() * sizeof(Vertex), &vertices_fb[0], GL_STATIC_DRAW);
}

VBO::VBO(const std::vector<Vertex>& input_vertices) {
	vertices = input_vertices;
	// Genereates buffers
	glGenBuffers(1, &id);
	// Binds buffers
	Bind();
	// Creates and initialize VBO data store
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

// Binds buffers
void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

// Unbinds buffers
void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Delete buffers
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