#pragma once
#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <iostream>

#include <vector>

#include "Shader_class.h"

#include "VAO_class.h"
#include "VBO_class.h"
#include "EBO_class.h"
#include "FBO_class.h"
#include "Texture_class.h"

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Texture_model {
	unsigned int id;
	std::string type;
	std::string filename;	// for future checking if texture already loaded
};

class Mesh {
	public:
		Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture_model> &textures);

		void Draw(Shader &shader);
	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture_model> textures;

		void setupMesh();

		VAO vao;
		VBO vbo;
		EBO ebo;
};

#endif