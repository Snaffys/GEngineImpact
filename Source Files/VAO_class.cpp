#include"VAO_class.h"

VAO::VAO(VBO& VBO) {
	// Generates and binds vertex arrays
	glGenVertexArrays(1, &id);
	Bind();
	// Binds VBO
	VBO.Bind();
	// Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);	
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glEnableVertexAttribArray(1);
	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	glEnableVertexAttribArray(2);

	// Fragment
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	// Texture
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//// Normals
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float))); 
	//glEnableVertexAttribArray(3);
	VBO.Unbind();
}

void VAO::FramebufferVAO(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFramebuffer), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFramebuffer), (void*)offsetof(VertexFramebuffer, tex_coords_fb));
	glEnableVertexAttribArray(1);
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

VAO::VAO(VAO&& move) noexcept{
	std::swap(id, move.id);
}

VAO& VAO::operator=(VAO&& other) noexcept{
	std::swap(id, other.id);
	return *this;
}

VAO::~VAO() {
	if (id != -1) {
		glDeleteVertexArrays(1, &id);
	}
}