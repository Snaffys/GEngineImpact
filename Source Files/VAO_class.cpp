#include"VAO_class.h"

VAO::VAO(VBO& VBO) {
	// Generates and binds vertex arrays
	glGenVertexArrays(1, &id);
	Bind();
	// Binds VBO
	VBO.Bind();

	// Vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(std::declval<Vertex>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(std::declval<Vertex>().normals) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	// Texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(std::declval<Vertex>().tex_coords) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	VBO.Unbind();
}

FramebufferVAO::FramebufferVAO(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(std::declval<VertexFramebuffer>().pos_fb) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(VertexFramebuffer), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(std::declval<VertexFramebuffer>().tex_coords_fb) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(VertexFramebuffer), (void*)offsetof(VertexFramebuffer, tex_coords_fb));
	VBO.Unbind();
}

CubemapVAO::CubemapVAO(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(std::declval<VertexCubemap>().pos_cbmp) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(VertexCubemap), (void*)0);
	VBO.Unbind();
}

void VAO::MatInstance(int index, VBO& VBO) {
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(index + 1);
	glVertexAttribPointer(index + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(index + 2);
	glVertexAttribPointer(index + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(index + 3);
	glVertexAttribPointer(index + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(index, 1);
	glVertexAttribDivisor(index + 1, 1);
	glVertexAttribDivisor(index + 2, 1);
	glVertexAttribDivisor(index + 3, 1);
	VBO.Unbind();
}

void VAO::VecInstance(int index, VBO& VBO) {
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glVertexAttribDivisor(index, 1);
	VBO.Unbind();
}

// Binds vertex array
void VAO::Bind() {
	glBindVertexArray(id);
}
// Unbinds vertex array
void VAO::Unbind() {
	glBindVertexArray(0);
}
// Deletes buffers
void VAO::Delete()
{
	glDeleteBuffers(1, &id);
}

VAO::VAO(VAO&& move) noexcept {
	std::swap(id, move.id);
}

VAO& VAO::operator=(VAO&& other) noexcept {
	std::swap(id, other.id);
	return *this;
}

VAO::~VAO() {
	if (id != -1)
		glDeleteVertexArrays(1, &id);
}