#include"VBO_class.h"


VBO::VBO(std::initializer_list<Vertex> coords) : vertices(coords) {
	// Genereates buffers
	glGenBuffers(1, &id);
	// Binds buffers
	Bind();
	// Creates and initialize VBO data store
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

VBO::VBO(std::initializer_list<VertexFramebuffer> coords) : vertices_fb(coords) {
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices_fb.size() * sizeof(VertexFramebuffer), &vertices_fb[0], GL_STATIC_DRAW);
}

VBO::VBO(std::initializer_list<VertexCubemap> coords) : vertices_cbmp(coords) {
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices_cbmp.size() * sizeof(VertexCubemap), &vertices_cbmp[0], GL_STATIC_DRAW);
}

VBO::VBO(const std::vector<Vertex>& input_vertices) {
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, input_vertices.size() * sizeof(Vertex), &input_vertices[0], GL_STATIC_DRAW);
}

VBO::VBO(const glm::mat4 model_matrices[], unsigned int amount) {
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &model_matrices[0], GL_STATIC_DRAW);
}

VBO::VBO(const glm::vec3 model_vec[], unsigned int amount) {
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::vec3), &model_vec[0], GL_STATIC_DRAW);
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

VBO::~VBO() {
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