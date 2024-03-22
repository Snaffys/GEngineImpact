#pragma once

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
	//Vertex(float x, float y, float z, float r, float g, float b, float s, float t, float n_x, float n_y, float n_z) 
	//	: pos(x,y,z), col(r,g,b), tex_coords(s,t), normals(n_x, n_y, n_z){};

	Vertex(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t)
		: pos(x, y, z), normals(n_x, n_y, n_z), tex_coords(s, t) {};



	glm::vec3 pos;
	//glm::vec3 col;
	glm::vec3 normals;
	glm::vec2 tex_coords;
};

struct VertexFramebuffer {
	VertexFramebuffer() = default;

	VertexFramebuffer(float x, float y, float s, float t)
		:pos_fb(x, y), tex_coords_fb(s, t) {};

	glm::vec2 pos_fb;
	glm::vec2 tex_coords_fb;
};

class VBO{
public:
	VBO() = default;

	VBO(std::initializer_list<Vertex> coords);

	VBO(std::initializer_list<VertexFramebuffer> coords);

	VBO(const std::vector<Vertex>& input_coords);

	VBO(VBO&& move) noexcept;
	VBO& operator=(VBO&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~VBO();
private:
	std::vector<Vertex> vertices;

	std::vector<VertexFramebuffer> vertices_fb;

	GLuint id = -1;
};

#endif