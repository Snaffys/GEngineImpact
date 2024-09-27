#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <initializer_list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
	Vertex() = default;

	Vertex(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t)
		: pos(x, y, z), normals(n_x, n_y, n_z), tex_coords(s, t) {};

	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 tex_coords;
};

struct VertexTan {
	VertexTan() = default;

	VertexTan(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t, float t_x, float t_y, float t_z, float b_x, float b_y, float b_z)
		: pos(x, y, z), normals(n_x, n_y, n_z), tex_coords(s, t), tangents(t_x, t_y, t_z), bitangents(b_x, b_y, b_z) {};

	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 tex_coords;
	glm::vec3 tangents;
	glm::vec3 bitangents;
};

struct VertexFramebuffer {
	VertexFramebuffer() = default;

	VertexFramebuffer(float x, float y, float s, float t)
		:pos_fb(x, y), tex_coords_fb(s, t) {};

	glm::vec2 pos_fb;
	glm::vec2 tex_coords_fb;
};

struct VertexCubemap {
	VertexCubemap() = default;

	VertexCubemap(float x, float y, float z)
		:pos_cbmp(x, y, z) {};

	glm::vec3 pos_cbmp;
};

class VBO {
public:
	VBO() = default;

	VBO(std::initializer_list<Vertex> coords);

	VBO(std::initializer_list<VertexTan> coords);

	VBO(std::initializer_list<VertexFramebuffer> coords);

	VBO(std::initializer_list<VertexCubemap> coords);

	VBO(const std::vector<Vertex>& input_coords);

	VBO(const std::vector<VertexTan>& input_coords);

	VBO(const glm::mat4 model_matrices[], unsigned int amount);

	VBO(const glm::vec3 model_vec[], unsigned int amount);

	VBO(VBO&& move) noexcept;
	VBO& operator=(VBO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~VBO();
private:
	std::vector<Vertex> vertices;

	std::vector<VertexTan> vertices_normals;

	std::vector<VertexFramebuffer> vertices_fb;

	std::vector<VertexCubemap> vertices_cbmp;

	GLuint id = NULL;
};

#endif